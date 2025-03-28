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
    if (reply == nullptr){
        qDebug() << "reply was empty";
        return;
    }

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        const auto addr = unit.startAddress(); 
        auto k = find(addr);
        auto d = m_ops.indexOf(addr);
        if(d != -1){
            m_ops.removeAt(d);
        } else {
            qDebug() << "address:" << addr <<" not found on stack";
        }
            
        if(k != -1){
            qDebug() << "address: " << addr << "found";
            updateSensor(m_dict[k], unit);
        } else {
            qDebug() << "address: " << addr << "not found";
        }
    } else {
        qDebug() << "Read error";
        QModbusDataUnit request = reply->request();
        const auto failedAddr = request.startAddress();
        auto d = m_ops.indexOf(failedAddr);
        if(d != -1){
            m_ops.removeAt(d);
        }

        if (reply->error() == QModbusDevice::ProtocolError) {
            qDebug() << QString("Read response error: %1 (Modbus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16);
        } else {
            qDebug() << QString("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16);
        }
    }

    reply->deleteLater();
    
    if(m_ops.size() == 0){
        qDebug() << "report ready emitting signal";
        emit reportReady();
    }else{
        qDebug() << "ops queue: " << m_ops.size() << " " << m_ops;
    }
}

void Deye::read(int startAddress, int numRegisters, int serverAddress){
    if (m_modbusDevice == nullptr){
        qWarning() << "modbusDevice not initialized";
        return;
    }

    if (auto *reply = m_modbusDevice->sendReadRequest(readRequest(startAddress, numRegisters), serverAddress)) {
        if (!reply->isFinished()){
            m_ops.push(startAddress);
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

int Deye::find(int address) const{
    for(int i = 0, end = m_dict.size(); i < end; ++i){
        if(m_dict[i].address == address){
            return i;
        }
    }

    return -1;
}

QVector<DeyeSensor> Deye::createSensorList() const {
    return {
        // Temperature Sensors (scaling /10)
        {"AC Temperature", "°C", "temperature", "ac_temp", "deye_ac_temp_001", 0.01, 541, SensorDataType::SHORT, 0},
        {"Battery Temperature", "°C", "temperature", "battery_temp", "deye_battery_temp_002", 0.01, 586, SensorDataType::SHORT, 0},
        {"DC Temperature", "°C", "temperature", "dc_temp", "deye_dc_temp_003", 0.01, 540, SensorDataType::SHORT, 0},

        // Battery Sensors
        {"Battery SOC", "%", "battery", "battery_soc", "deye_battery_capacity_004", 1.0, 588, SensorDataType::SHORT, 0},
        //{"Battery SOH", "%", "battery", "battery_soh", "deye_battery_soh_005", 1.0, 10006, SensorDataType::SHORT, 0},
        {"Battery Current", "mA", "current", "battery_current", "deye_battery_current_006", 0.01, 591, SensorDataType::SSHORT, 0},
        {"Battery Power", "W", "power", "battery_power", "deye_battery_power_007", 1.0, 590, SensorDataType::SSHORT, 0},
        {"Battery Voltage", "V", "voltage", "battery_voltage", "deye_battery_voltage_008", 0.01, 587, SensorDataType::USHORT, 0},

        // Current Sensors (scaling /10)
        {"Current L1", "A", "current", "current_l1", "deye_current_l1_009", 0.01, 630, SensorDataType::SSHORT, 0},
        {"Current L2", "A", "current", "current_l2", "deye_current_l2_010", 0.01, 631, SensorDataType::SSHORT, 0},
        {"Current L3", "A", "current", "current_l3", "deye_current_l3_011", 0.01, 632, SensorDataType::SSHORT, 0},

        // Voltage Sensors (scaling /10)
        {"Grid Voltage L1", "V", "voltage", "grid_voltage_l1", "deye_grid_voltage_l1_012", 0.1, 598, SensorDataType::SHORT, 0},
        {"Grid Voltage L2", "V", "voltage", "grid_voltage_l2", "deye_grid_voltage_l2_013", 0.1, 599, SensorDataType::SHORT, 0},
        {"Grid Voltage L3", "V", "voltage", "grid_voltage_l3", "deye_grid_voltage_l3_014", 0.1, 600, SensorDataType::SHORT, 0},
        {"Load Voltage L1", "V", "voltage", "load_voltage_l1", "deye_load_voltage_l1_015", 0.1, 644, SensorDataType::USHORT, 0},
        {"Load Voltage L2", "V", "voltage", "load_voltage_l2", "deye_load_voltage_l2_016", 0.1, 645, SensorDataType::USHORT, 0},
        {"Load Voltage L3", "V", "voltage", "load_voltage_l3", "deye_load_voltage_l3_017", 0.1, 646, SensorDataType::USHORT, 0},
        {"PV1 Voltage", "V", "voltage", "pv1_voltage", "deye_pv1_voltage_018", 0.1, 676, SensorDataType::SHORT, 0},
        {"PV2 Voltage", "V", "voltage", "pv2_voltage", "deye_pv2_voltage_019", 0.1, 678, SensorDataType::SHORT, 0},

        // Current Sensors (scaling /10)
        {"PV1 Current", "A", "current", "pv1_current", "deye_pv1_current_020", 0.1, 677, SensorDataType::SHORT, 0},
        {"PV2 Current", "A", "current", "pv2_current", "deye_pv2_current_021", 0.1, 679, SensorDataType::SHORT, 0},

        // Power Sensors
        {"PV1 Power", "W", "power", "pv1_input_power", "deye_pv1_power_022", 1.0, 672, SensorDataType::SHORT, 0},
        {"PV2 Power", "W", "power", "pv2_power", "deye_pv2_power_023", 1.0, 673, SensorDataType::SHORT, 0},
        {"Day Load Power", "W", "power", "day_load_power", "deye_day_load_power_024", 1.0, 526, SensorDataType::SHORT, 0},
        {"External CT L1 Power", "W", "power", "external_ct_l1_power", "deye_external_ct_l1_power_025", 1.0, 616, SensorDataType::SSHORT, 0},
        {"External CT L2 Power", "W", "power", "external_ct_l2_power", "deye_external_ct_l2_power_026", 1.0, 617, SensorDataType::SSHORT, 0},
        {"External CT L3 Power", "W", "power", "external_ct_l3_power", "deye_external_ct_l3_power_027", 1.0, 618, SensorDataType::SSHORT, 0},
        {"Gen Port Power", "W", "power", "gen_port_power", "deye_gen_port_power_028", 1.0, 667, SensorDataType::SHORT, 0},
        {"GridSell Power", "W", "power", "gridsell_power", "deye_gridsell_power_029", 1.0, 525, SensorDataType::SHORT, 0},
        {"Internal CT L1 Power", "W", "power", "internal_ct_l1_power", "deye_internal_ct_l1_power_030", 1.0, 604, SensorDataType::SSHORT, 0},
        {"Internal CT L2 Power", "W", "power", "internal_ct_l2_power", "deye_internal_ct_l2_power_031", 1.0, 605, SensorDataType::SSHORT, 0},
        {"Internal CT L3 Power", "W", "power", "internal_ct_l3_power", "deye_internal_ct_l3_power_032", 1.0, 606, SensorDataType::SSHORT, 0},
        {"Inverter L1 Power", "W", "power", "inverter_l1_power", "deye_inverter_l1_power_033", 1.0, 633, SensorDataType::SSHORT, 0},
        {"Inverter L2 Power", "W", "power", "inverter_l2_power", "deye_inverter_l2_power_034", 1.0, 634, SensorDataType::SSHORT, 0},
        {"Inverter L3 Power", "W", "power", "inverter_l3_power", "deye_inverter_l3_power_035", 1.0, 635, SensorDataType::SSHORT, 0},
        {"Load L1 Power", "W", "power", "load_l1_power", "deye_load_l1_power_036", 1.0, 650, SensorDataType::SSHORT, 0},
        {"Load L2 Power", "W", "power", "load_l2_power", "deye_load_l2_power_037", 1.0, 651, SensorDataType::SSHORT, 0},
        {"Load L3 Power", "W", "power", "load_l3_power", "deye_load_l3_power_038", 1.0, 652, SensorDataType::SSHORT, 0},
        {"Total Grid Power", "W", "power", "total_grid_power", "deye_total_grid_power_039", 1.0, 625, SensorDataType::SHORT, 0},
        {"Total Load Power", "W", "power", "total_load_power", "deye_total_load_power_040", 1.0, 653, SensorDataType::SSHORT, 0},
        {"Total Active IN Power", "W", "power", "total_active_in_power", "deye_total_active_in_power_041", 1.0, 607, SensorDataType::SSHORT, 0},

        // Energy Sensors (scaling /100)
        {"Daily Battery Charge", "kWh", "energy", "daily_battery_charge", "deye_daily_battery_charge_042", 0.1, 514, SensorDataType::SHORT, 0},
        {"Daily Battery Discharge", "kWh", "energy", "daily_battery_discharge", "deye_daily_battery_discharge_043", 0.1, 515, SensorDataType::SHORT, 0},
        {"Daily Energy Bought", "kWh", "energy", "daily_energy_bought", "deye_daily_energy_bought_044", 0.1, 520, SensorDataType::SHORT, 0},
        {"Daily Energy Sold", "kWh", "energy", "daily_energy_sold", "deye_daily_energy_sold_045", 0.1, 521, SensorDataType::SHORT, 0},
        {"Daily Load Consumption", "kWh", "energy", "daily_load_consumption", "deye_daily_load_consumption_046", 0.01, 526, SensorDataType::SHORT, 0},
        {"Daily Production", "kWh", "energy", "daily_production", "deye_daily_production_047", 0.1, 529, SensorDataType::SHORT, 0},
        {"Total Battery Charge", "kWh", "energy", "total_battery_charge", "deye_total_battery_charge_048", 0.01, 516, SensorDataType::DWORD, 0},
//        {"Total Battery Charge", "kWh", "energy", "total_battery_charge", "deye_total_battery_charge_049", 0.01, 517},
        {"Total Battery Discharge", "kWh", "energy", "total_battery_discharge", "deye_total_battery_discharge_050", 0.01, 518, SensorDataType::DWORD, 0},
//        {"Total Battery Discharge", "kWh", "energy", "total_battery_discharge", "deye_total_battery_discharge_051", 0.01, 519, SensorDataType::SHORT, 0},
        {"Total Energy Bought", "kWh", "energy", "total_energy_bought", "deye_total_energy_bought_052", 0.1, 522, SensorDataType::DWORD, 0},
//        {"Total Energy Bought", "kWh", "energy", "total_energy_bought", "deye_total_energy_bought_053", 0.01, 523},
        {"Total Energy Sold", "kWh", "energy", "total_energy_sold", "deye_total_energy_sold_054", 0.01, 524, SensorDataType::DWORD, 0},
//        {"Total Energy Sold", "kWh", "energy", "total_energy_sold", "deye_total_energy_sold_054", 0.01, 525},
        {"Total Load Consumption", "kWh", "energy", "total_load_consumption", "deye_total_load_consumption_055", 0.1, 527, SensorDataType::DWORD, 0},
//        {"Total Load Consumption", "kWh", "energy", "total_load_consumption", "deye_total_load_consumption_056", 0.01, 528},
        {"Total Production", "kWh", "energy", "total_production", "deye_total_production_057", 0.1, 534, SensorDataType::DWORD, 0},
//        {"Total Production", "kWh", "energy", "total_production", "deye_total_production_058", 0.01, 535}
    };
}

const QVector<DeyeSensor>& Deye::sensors() const{
    return m_dict;
}

float Deye::sensorValue(const /*signed*/ qint16 data, float scale) const{
    return static_cast<float>(data)*scale;
}

float Deye::sensorValue(const /*unsigned*/ quint16 data, float scale) const{
    return static_cast<float>(data)*scale;
}

float Deye::sensorValue(const /*signed*/ qint16 low, const qint16 high, float scale) const{
    SensorData tmp;
    tmp.words.low = low;
    tmp.words.high = high;
    return static_cast<float>(tmp.data)*scale;
}

void Deye::updateSensor(DeyeSensor &sensor, const QModbusDataUnit& unit) {

    switch(sensor.type){
        case SensorDataType::SSHORT:
        case SensorDataType::SHORT:
            sensor.data = sensorValue(static_cast<qint16>(unit.value(0)), sensor.scalingFactor);
            break;
        case SensorDataType::USHORT:
            sensor.data = sensorValue(static_cast<quint16>(unit.value(0)), sensor.scalingFactor);
            break;
        case SensorDataType::DWORD:
            if(unit.valueCount() != 2){
                qCritical() << "Invalid values count for DWORD";
                return;
            }
            sensor.data = sensorValue(static_cast<qint16>(unit.value(0)), static_cast<qint16>(unit.value(1)), sensor.scalingFactor);
            break;
    }

    if(m_client){
        //if (!m_client || m_client->state() != QMqttClient::Connected) {
        //    qWarning() << "MQTT client not connected!";
        //    return;
        //}

        qDebug() << "mqtt client found updating sensor value";
        const QString topic = "deye/sensor/" + sensor.topicSuffix + "/state";
        qDebug() << "publishing" << topic << " value: " <<  QByteArray::number(sensor.data, 'f', 2);

        m_client->publish(
            QMqttTopicName(topic),
            QByteArray::number(sensor.data, 'f', 2),  // 2 decimal places
            1,  // QoS 1
            true
        );
    } else {
        qDebug() << "mqtt client not found"; 
    }

    if(m_model){
        qDebug() << "model found updating sensor value";
        (*m_model)[sensor.topicSuffix] = sensor.data;
    } else {
        qDebug() << "model not found";
    }
}

void Deye::readReport(){
    qDebug() << "Performing read request, current ops: " << m_ops.size();
    for(const auto& sensor : m_dict){
        switch(sensor.type){
            case SensorDataType::SHORT:
            case SensorDataType::USHORT:
            case SensorDataType::SSHORT:
                read(sensor.address, 1);
                break;
            case SensorDataType::DWORD:
                read(sensor.address, 2);
                break;
        }
    }
}
