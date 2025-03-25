#include "deye.h"

Deye::Deye(const Settings& settings, QJsonObject* model, QMqttClient *client, QObject* parent)
    : m_modbusDevice(new QModbusRtuSerialClient(parent))
    , m_model(model)
    , m_client(client){
    if(m_modbusDevice == nullptr){
        qDebug() << "Failed to create modbus device";
        return;
    }

    m_dict = createSensorList();

    m_modbusDevice->setConnectionParameter(
        QModbusDevice::SerialPortNameParameter,
        settings.device);

    m_modbusDevice->setConnectionParameter(
        QModbusDevice::SerialParityParameter,
        SerialPort_parity_from_string(settings.parity)
    );

    m_modbusDevice->setConnectionParameter(
        QModbusDevice::SerialBaudRateParameter,
        settings.baud
    );

    m_modbusDevice->setConnectionParameter(
        QModbusDevice::SerialDataBitsParameter,
        settings.dataBits
    );

    m_modbusDevice->setConnectionParameter(
        QModbusDevice::SerialStopBitsParameter,
        settings.stopBits
    );

    m_modbusDevice->setTimeout(settings.responseTime);
    m_modbusDevice->setNumberOfRetries(settings.numberOfRetries);

    QObject::connect(m_modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        qDebug() << this->m_modbusDevice->errorString();
    });

    QObject::connect(m_modbusDevice, &QModbusClient::stateChanged, [](int state){
        qDebug() << "ModBus device state changed: " << ModBus_state_string(state);
    });
}

QModbusDataUnit Deye::readRequest(int startAddress, int numRegisters){
    return QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numRegisters);
}

void Deye::onReadReady(QModbusReply* reply){
    if (!reply){
        qDebug() << "reply was empty";
        return;
    }

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (qsizetype i = 0, total = unit.valueCount(); i < total; ++i) {
            quint16 entry = unit.value(i);
            auto k = find(unit.startAddress());
            if(k.has_value()){
                (*m_model)[k.value().uniqueId] = static_cast<float>(entry)*k.value().scalingFactor;
                updateSensor(k.value(), unit.value(i));
            } else {
                qDebug() << "address: " << unit.startAddress() << "not found";
            }
        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        qDebug() << QString("Read response error: %1 (Modbus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16);
    } else {
        qDebug() << QString("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16);
    }

    reply->deleteLater();
}

void Deye::read(int startAddress, int numRegisters, int serverAddress){
    if (m_modbusDevice == nullptr){
        qWarning() << "modbusDevice not initialized";
        return;
    }

    if (auto *reply = m_modbusDevice->sendReadRequest(readRequest(startAddress, numRegisters), serverAddress)) {
        if (!reply->isFinished()){
            QObject::connect(reply, &QModbusReply::finished, [this, reply](){
                this->onReadReady(reply);
            });
        } else {
            delete reply; // broadcast replies return immediately
        }
    } else {
        qCritical() << QString("Read error: %1").arg(m_modbusDevice->errorString());
    }
}

std::optional<const DeyeSensor> Deye::find(int address) const{
    for(const auto& sensor : m_dict){
        if(sensor.address == address){
            return std::optional<const DeyeSensor>(sensor);
        }
    }

    return std::nullopt;
}

QVector<DeyeSensor> Deye::createSensorList() const {
    return {
        // Temperature Sensors (scaling /10)
        {"AC Temperature", "°C", "temperature", "ac_temp", "deye_ac_temp_001", 0.1, 541},
        {"Battery Temperature", "°C", "temperature", "battery_temp", "deye_battery_temp_002", 0.1, 586},
        {"DC Temperature", "°C", "temperature", "dc_temp", "deye_dc_temp_003", 0.1, 540},

        // Battery Sensors
        {"Battery Capacity", "%", "battery", "battery_capacity", "deye_battery_capacity_004", 1.0, 588},
        {"Battery SOC", "%", "battery", "battery_soc", "deye_battery_soc_005", 1.0, 588},
        {"Battery Current", "mA", "current", "battery_current", "deye_battery_current_006", 1.0, 591},
        {"Battery Power", "W", "power", "battery_power", "deye_battery_power_007", 1.0, 590},
        {"Battery Voltage", "V", "voltage", "battery_voltage", "deye_battery_voltage_008", 0.1, 587},

        // Current Sensors (scaling /10)
        {"Current L1", "A", "current", "current_l1", "deye_current_l1_009", 0.1, 630},
        {"Current L2", "A", "current", "current_l2", "deye_current_l2_010", 0.1, 631},
        {"Current L3", "A", "current", "current_l3", "deye_current_l3_011", 0.1, 632},

        // Voltage Sensors (scaling /10)
        {"Grid Voltage L1", "V", "voltage", "grid_voltage_l1", "deye_grid_voltage_l1_012", 0.1, 598},
        {"Grid Voltage L2", "V", "voltage", "grid_voltage_l2", "deye_grid_voltage_l2_013", 0.1, 599},
        {"Grid Voltage L3", "V", "voltage", "grid_voltage_l3", "deye_grid_voltage_l3_014", 0.1, 600},
        {"Load Voltage L1", "V", "voltage", "load_voltage_l1", "deye_load_voltage_l1_015", 0.1, 644},
        {"Load Voltage L2", "V", "voltage", "load_voltage_l2", "deye_load_voltage_l2_016", 0.1, 645},
        {"Load Voltage L3", "V", "voltage", "load_voltage_l3", "deye_load_voltage_l3_017", 0.1, 646},
        {"PV1 Voltage", "V", "voltage", "pv1_voltage", "deye_pv1_voltage_018", 0.1, 676},
        {"PV2 Voltage", "V", "voltage", "pv2_voltage", "deye_pv2_voltage_019", 0.1, 678},

        // Current Sensors (scaling /10)
        {"PV1 Current", "A", "current", "pv1_current", "deye_pv1_current_020", 0.1, 677},
        {"PV2 Current", "A", "current", "pv2_current", "deye_pv2_current_021", 0.1, 679},

        // Power Sensors
        {"PV1 Power", "W", "power", "pv1_input_power", "deye_pv1_power_022", 1.0, 672},
        {"PV2 Power", "W", "power", "pv2_power", "deye_pv2_power_023", 1.0, 673},
        {"Day Load Power", "W", "power", "day_load_power", "deye_day_load_power_024", 1.0, 526},
        {"External CT L1 Power", "W", "power", "external_ct_l1_power", "deye_external_ct_l1_power_025", 1.0, 616},
        {"External CT L2 Power", "W", "power", "external_ct_l2_power", "deye_external_ct_l2_power_026", 1.0, 617},
        {"External CT L3 Power", "W", "power", "external_ct_l3_power", "deye_external_ct_l3_power_027", 1.0, 618},
        {"Gen Port Power", "W", "power", "gen_port_power", "deye_gen_port_power_028", 1.0, 667},
        {"GridSell Power", "W", "power", "gridsell_power", "deye_gridsell_power_029", 1.0, 525},
        {"Internal CT L1 Power", "W", "power", "internal_ct_l1_power", "deye_internal_ct_l1_power_030", 1.0, 604},
        {"Internal CT L2 Power", "W", "power", "internal_ct_l2_power", "deye_internal_ct_l2_power_031", 1.0, 605},
        {"Internal CT L3 Power", "W", "power", "internal_ct_l3_power", "deye_internal_ct_l3_power_032", 1.0, 606},
        {"Inverter L1 Power", "W", "power", "inverter_l1_power", "deye_inverter_l1_power_033", 1.0, 633},
        {"Inverter L2 Power", "W", "power", "inverter_l2_power", "deye_inverter_l2_power_034", 1.0, 634},
        {"Inverter L3 Power", "W", "power", "inverter_l3_power", "deye_inverter_l3_power_035", 1.0, 635},
        {"Load L1 Power", "W", "power", "load_l1_power", "deye_load_l1_power_036", 1.0, 650},
        {"Load L2 Power", "W", "power", "load_l2_power", "deye_load_l2_power_037", 1.0, 651},
        {"Load L3 Power", "W", "power", "load_l3_power", "deye_load_l3_power_038", 1.0, 652},
        {"Total Grid Power", "W", "power", "total_grid_power", "deye_total_grid_power_039", 1.0, 625},
        {"Total Load Power", "W", "power", "total_load_power", "deye_total_load_power_040", 1.0, 653},
        {"Total Active IN Power", "W", "power", "total_active_in_power", "deye_total_active_in_power_041", 1.0, 607},

        // Energy Sensors (scaling /100)
        {"Daily Battery Charge", "kWh", "energy", "daily_battery_charge", "deye_daily_battery_charge_042", 0.01, 514},
        {"Daily Battery Discharge", "kWh", "energy", "daily_battery_discharge", "deye_daily_battery_discharge_043", 0.01, 515},
        {"Daily Energy Bought", "kWh", "energy", "daily_energy_bought", "deye_daily_energy_bought_044", 0.01, 520},
        {"Daily Energy Sold", "kWh", "energy", "daily_energy_sold", "deye_daily_energy_sold_045", 0.01, 521},
        {"Daily Load Consumption", "kWh", "energy", "daily_load_consumption", "deye_daily_load_consumption_046", 0.01, 526},
        {"Daily Production", "kWh", "energy", "daily_production", "deye_daily_production_047", 0.01, 529},
        {"Total Battery Charge", "kWh", "energy", "total_battery_charge", "deye_total_battery_charge_048", 0.01, 516},
//        {"Total Battery Charge", "kWh", "energy", "total_battery_charge", "deye_total_battery_charge_049", 0.01, 517},
        {"Total Battery Discharge", "kWh", "energy", "total_battery_discharge", "deye_total_battery_discharge_050", 0.01, 518},
        {"Total Battery Discharge", "kWh", "energy", "total_battery_discharge", "deye_total_battery_discharge_051", 0.01, 519},
        {"Total Energy Bought", "kWh", "energy", "total_energy_bought", "deye_total_energy_bought_052", 0.01, 522},
//        {"Total Energy Bought", "kWh", "energy", "total_energy_bought", "deye_total_energy_bought_053", 0.01, 523},
        {"Total Energy Sold", "kWh", "energy", "total_energy_sold", "deye_total_energy_sold_054", 0.01, 524},
//        {"Total Energy Sold", "kWh", "energy", "total_energy_sold", "deye_total_energy_sold_054", 0.01, 525},
        {"Total Load Consumption", "kWh", "energy", "total_load_consumption", "deye_total_load_consumption_055", 0.01, 527},
//        {"Total Load Consumption", "kWh", "energy", "total_load_consumption", "deye_total_load_consumption_056", 0.01, 528},
        {"Total Production", "kWh", "energy", "total_production", "deye_total_production_057", 0.01, 534},
//        {"Total Production", "kWh", "energy", "total_production", "deye_total_production_058", 0.01, 535}
    };
}

const QVector<DeyeSensor>& Deye::sensors() const{
    return m_dict;
}

void Deye::updateSensor(const DeyeSensor &sensor, float rawValue) {
    if (!m_client || m_client->state() != QMqttClient::Connected) {
        qWarning() << "MQTT client not connected!";
        return;
    }

    const float scaledValue = rawValue * sensor.scalingFactor;
    const QString topic = "deye/sensor/" + sensor.topicSuffix + "/state";

    qDebug() << "publishing" << topic << " value: " <<  QByteArray::number(scaledValue, 'f', 2) << "raw: " << rawValue;

    m_client->publish(
        QMqttTopicName(topic),
        QByteArray::number(scaledValue, 'f', 2),  // 2 decimal places
        1,  // QoS 1
        true
    );
}

void Deye::readReport(){
    for(const auto& sensor : m_dict){
        read(sensor.address, 1);
    }
}
