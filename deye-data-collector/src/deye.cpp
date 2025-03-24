#include "deye.h"

auto DICT_find(const QVector<DeyeSensor>& dict, const QString& name) -> int{
    for(int i = 0; i < dict.size(); ++i){
        if(dict[i].name == name){
            return i;
        }
    }

    return -1;
}

void updateSensor(QMqttClient* mqttClient, const DeyeSensor &sensor, float rawValue) {
    if (!mqttClient || mqttClient->state() != QMqttClient::Connected) {
        qWarning() << "MQTT client not connected!";
        return;
    }

    const float scaledValue = rawValue * sensor.scalingFactor;
    const QString topic = "deye/sensor/" + sensor.topicSuffix + "/state";

    qDebug() << "publishing" << topic << " " <<  QByteArray::number(scaledValue, 'f', 2);

    mqttClient->publish(
        QMqttTopicName(topic),
        QByteArray::number(scaledValue, 'f', 2),  // 2 decimal places
        1,  // QoS 1
        false
    );
}

Deye::Deye(const Settings& settings, QJsonObject* model, QMqttClient *client, const QVector<DeyeSensor>& dict, QObject* parent)
    : m_modbusDevice(new QModbusRtuSerialClient(parent))
    , m_model(model)
    , m_client(client)
    , m_dict(dict){
    if(m_modbusDevice == nullptr){
        qDebug() << "Failed to create modbus device";
        return;
    }

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

void Deye::onReadReady(QModbusReply* reply, const ValueModifier& mod){
    if (!reply){
        qDebug() << "reply was empty";
        return;
    }

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (qsizetype i = 0, total = unit.valueCount(); i < total; ++i) {
            quint16 entry = unit.value(i);
            if(mod.scale != 1.0f){
                //qDebug() << mod.name << "Register Address:" << unit.startAddress()+i << "value:" << entry << mod.unit;
                (*m_model)[mod.name] = entry;
            } else {
                const float tmp = static_cast<float>(entry)*mod.scale;
                //qDebug() << mod.name << "Register Address:" << unit.startAddress()+i << "value:" << tmp << mod.unit;
                (*m_model)[mod.name] = tmp;
            }

            auto k = DICT_find(m_dict, mod.name);
            if(k != -1){
                qDebug() << mod.name << "found";
                updateSensor(m_client, m_dict[k], unit.value(i));
            } else {
                qDebug() << mod.name << "not found";
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

void Deye::read(int startAddress, int numRegisters, const ValueModifier& mod, int serverAddr){
    if (m_modbusDevice == nullptr){
        qWarning() << "modbusDevice not initialized";
        return;
    }

    if (auto *reply = m_modbusDevice->sendReadRequest(readRequest(startAddress, numRegisters), serverAddr)) {
        if (!reply->isFinished()){
            QObject::connect(reply, &QModbusReply::finished, [this, reply, mod](){
                this->onReadReady(reply, mod);
            });
        } else {
            delete reply; // broadcast replies return immediately
        }
    } else {
        qCritical() << QString("Read error: %1").arg(m_modbusDevice->errorString());
    }
}

void Deye::readReport(){
    read(672, 1, {1, "W", "PV1 input power"});
    read(673, 1, {1, "W", "PV2 Power"});
    read(676, 1, {0.1, "V", "PV1 Voltage"});
    read(678, 1, {0.1, "V", "PV2 Voltage"});
    read(677, 1, {0.1, "A", "PV1 Current"});
    read(679, 1, {0.1, "A", "PV2 Current"});
    read(529, 1, {0.1, "kWh", "Daily Production"});
    read(534, 1, {0.1, "kWh", "Total Production"});
    read(535, 1, {0.1, "kWh", "Total Production"});
    read(625, 1, {1, "W", "Total Grid Power"});
    read(598, 1, {0.1, "V", "Grid Voltage L1"});
    read(599, 1, {0.1, "V", "Grid Voltage L2"});
    read(600, 1, {0.1, "V", "Grid Voltage L3"});
    read(604, 1, {1, "W", "Internal CT L1 Power"});
    read(605, 1, {1, "W", "Internal CT L2 Power"});
    read(606, 1, {1, "W", "Internal CT L3 Power"});
    read(607, 1, {1, "W", "Total active IN Power"});
    read(616, 1, {1, "W", "External CT L1 Power"});
    read(617, 1, {1, "W", "External CT L2 Power"});
    read(618, 1, {1, "W", "External CT L3 Power"});
    read(520, 1, {0.1, "kWh", "Daily Energy Bought"});
    read(522, 1, {0.1, "kWh", "Total Energy Bought"});
    read(523, 1, {0.1, "kWh", "Total Energy Bought"});
    read(521, 1, {0.1, "kWh", "Daily Energy Sold"});
    read(524, 1, {0.1, "kWh", "Total Energy Sold"});
    read(525, 1, {0.1, "kWh", "Total Energy Sold"});
    read(526, 1, {0.1, "kWh", "Day Load Power"});
    read(525, 1, {0.1, "kWh", "GridSell Power"});
    read(667, 1, {1, "W", "Gen Port Power"});
    read(630, 1, {0.1, "A", "Current L1"});
    read(631, 1, {0.1, "A", "Current L2"});
    read(632, 1, {0.1, "A", "Current L3"});
    read(633, 1, {1, "W", "Inverter L1 Power"});
    read(634, 1, {1, "W", "Inverter L2 Power"});
    read(635, 1, {1, "W", "Inverter L3 Power"});
    read(540, 1, {0.1, "C", "DC Temperature"});
    read(541, 1, {0.1, "C", "AC Temperature"});
    read(514, 1, {0.1, "kWh", "Daily Battery Charge"});
    read(515, 1, {0.1, "kWh", "Daily Battery Discharge"});
    read(516, 1, {0.1, "kWh", "Total Battery Charge"});
    read(517, 1, {0.1, "kWh", "Total Battery Charge"});
    read(518, 1, {0.1, "kWh", "Total Battery Discharge"});
    read(519, 1, {0.1, "kWh", "Total Battery Discharge"});
    read(590, 1, {1, "W", "Battery Power"});
    read(587, 1, {0.01, "V", "Battery Voltage"});
    read(588, 1, {1, "%", "Battery SOC"});
    read(591, 1, {0.01, "A", "Battery Current"});
    read(586, 1, {0.1, "C", "Battery Temperature"});
    read(653, 1, {1, "W", "Total Load Power"});
    read(650, 1, {1, "W", "Load L1 Power"});
    read(651, 1, {1, "W", "Load L2 Power"});
    read(652, 1, {1, "W", "Load L3 Power"});
    read(644, 1, {0.1, "V", "Load Voltage L1"});
    read(645, 1, {0.1, "V", "Load Voltage L2"});
    read(646, 1, {0.1, "V", "Load Voltage L3"});
    read(526, 1, {0.1, "kWh", "Daily Load Consumption"});
    read(527, 1, {0.1, "kWh", "Total Load Consumption"});
    read(528, 1, {0.1, "kWh", "Total Load Consumption"});
    read(588, 1, {1, "%", "Battery Capacity"});
}
