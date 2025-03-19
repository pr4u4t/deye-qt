#include <QVariant>
#include <QTimer>
#include <QDateTime>

#include "main.h"

auto ModBus_parity_from_string(const QString& parity) ->  QSerialPort::Parity{
    if(parity == "none"){
        return QSerialPort::NoParity;
    }
    
    if(parity == "even"){
        return QSerialPort::EvenParity;
    } 
    
    if(parity == "odd"){
        return QSerialPort::OddParity;
    }
    
    if(parity == "mark"){
        return QSerialPort::MarkParity;
    }
    
    if(parity == "space"){
        return QSerialPort::SpaceParity;
    }
    
    qDebug() << "No valid parity defaulting to none";

    return QSerialPort::NoParity;
}

auto ModBus_state_string(int state) -> QString {
    switch(state){
        case QModbusDevice::UnconnectedState:
            return "The device is disconnected.";
        case QModbusDevice::ConnectingState:
            return "The device is being connected.";
        case QModbusDevice::ConnectedState:
            return "The device is connected to the Modbus network.";
        case QModbusDevice::ClosingState:
            return "The device is being closed.";
    }

    return "Unknown state";
}

QModbusDataUnit ModBus_readRequest(int startAddress, int numRegisters){
    return QModbusDataUnit(QModbusDataUnit::HoldingRegisters, startAddress, numRegisters);
}

void ModBus_onReadReady(QModbusReply* reply, const ValueModifier& mod){
    if (!reply){
        qDebug() << "reply was empty";
        return;
    }

    if (reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for (qsizetype i = 0, total = unit.valueCount(); i < total; ++i) {
            quint16 entry = unit.value(i);
            if(mod.scale != 1.0f){
                qDebug() << mod.name << "Register Address:" << unit.startAddress()+i << "value:" << entry << mod.unit;
            } else {
                const float tmp = static_cast<float>(entry)*mod.scale;
                qDebug() << mod.name << "Register Address:" << unit.startAddress()+i << "value:" << tmp << mod.unit;
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

void ModBus_read(QModbusClient* modbusDevice, int startAddress, int numRegisters, const ValueModifier& mod = ValueModifier(), int serverAddr = 1){
    if (!modbusDevice){
        return;
    }

//! [read_data_1]
    if (auto *reply = modbusDevice->sendReadRequest(ModBus_readRequest(startAddress, numRegisters), serverAddr)) {
        if (!reply->isFinished()){
            QObject::connect(reply, &QModbusReply::finished, [reply, mod](){
                ModBus_onReadReady(reply, mod);
            });
        }else{
            delete reply; // broadcast replies return immediately
        }
    } else {
        qDebug() << QString("Read error: %1").arg(modbusDevice->errorString());
    }
}

auto ModBus_create(const ModBusSettings& settings, QObject* parent = nullptr) -> QModbusClient*{
    auto modbusDevice = new QModbusRtuSerialClient(parent);

    if(modbusDevice == nullptr){
        qDebug() << "Failed to create modbus device";
        return nullptr;
    }

    modbusDevice->setConnectionParameter(
        QModbusDevice::SerialPortNameParameter,
                settings.m_port);

    modbusDevice->setConnectionParameter(
        QModbusDevice::SerialParityParameter,
        settings.m_parity
    );
            
    modbusDevice->setConnectionParameter(
        QModbusDevice::SerialBaudRateParameter,
        settings.m_baud
    );
            
    modbusDevice->setConnectionParameter(
        QModbusDevice::SerialDataBitsParameter,
        settings.m_dataBits
    );
            
    modbusDevice->setConnectionParameter(
        QModbusDevice::SerialStopBitsParameter,
        settings.m_stopBits
    );

    modbusDevice->setTimeout(settings.m_responseTime);
    modbusDevice->setNumberOfRetries(settings.m_numberOfRetries);

    QObject::connect(modbusDevice, &QModbusClient::errorOccurred, [modbusDevice](QModbusDevice::Error) {
        qDebug() << modbusDevice->errorString();
    });

    QObject::connect(modbusDevice, &QModbusClient::stateChanged, [modbusDevice](int state){
        qDebug() << "ModBus device state changed: " << ModBus_state_string(state);
    });

    return modbusDevice;
}

void ModBus_read_report(QModbusClient* modbus){
    ModBus_read(modbus, 672, 1, {1, "W", "PV1 input power"});
    ModBus_read(modbus, 673, 1, {1, "W", "PV2 Power"});
    ModBus_read(modbus, 676, 1, {0.1, "V", "PV1 Voltage"});
    ModBus_read(modbus, 678, 1, {0.1, "V", "PV2 Voltage"});
    ModBus_read(modbus, 677, 1, {0.1, "A", "PV1 Current"});
    ModBus_read(modbus, 679, 1, {0.1, "A", "PV2 Current"});
    ModBus_read(modbus, 529, 1, {0.1, "kWh", "Daily Production"});
    ModBus_read(modbus, 534, 1, {0.1, "kWh", "Total Production"});
    ModBus_read(modbus, 535, 1, {0.1, "kWh", "Total Production"});
    ModBus_read(modbus, 625, 1, {1, "W", "Total Grid Power"});
    ModBus_read(modbus, 598, 1, {0.1, "V", "Grid Voltage L1"});
    ModBus_read(modbus, 599, 1, {0.1, "V", "Grid Voltage L2"});
    ModBus_read(modbus, 600, 1, {0.1, "V", "Grid Voltage L3"});
    ModBus_read(modbus, 604, 1, {1, "W", "Internal CT L1 Power"});
    ModBus_read(modbus, 605, 1, {1, "W", "Internal CT L2 Power"});
    ModBus_read(modbus, 606, 1, {1, "W", "Internal CT L3 Power"});
    ModBus_read(modbus, 607, 1, {1, "W", "Total active IN Power"});
    ModBus_read(modbus, 616, 1, {1, "W", "External CT L1 Power"});
    ModBus_read(modbus, 617, 1, {1, "W", "External CT L2 Power"});
    ModBus_read(modbus, 618, 1, {1, "W", "External CT L3 Power"});
    ModBus_read(modbus, 520, 1, {0.1, "kWh", "Daily Energy Bought"});
    ModBus_read(modbus, 522, 1, {0.1, "kWh", "Total Energy Bought"});
    ModBus_read(modbus, 523, 1, {0.1, "kWh", "Total Energy Bought"});
    ModBus_read(modbus, 521, 1, {0.1, "kWh", "Daily Energy Sold"});
    ModBus_read(modbus, 524, 1, {0.1, "kWh", "Total Energy Sold"});
    ModBus_read(modbus, 525, 1, {0.1, "kWh", "Total Energy Sold"});
    ModBus_read(modbus, 526, 1, {0.1, "kWh", "Day Load Power"});
    ModBus_read(modbus, 525, 1, {0.1, "kWh", "GridSell Power"});
    ModBus_read(modbus, 667, 1, {1, "W", "Gen Port Power"});
    ModBus_read(modbus, 630, 1, {0.1, "A", "Current L1"});
    ModBus_read(modbus, 631, 1, {0.1, "A", "Current L2"});
    ModBus_read(modbus, 632, 1, {0.1, "A", "Current L3"});
    ModBus_read(modbus, 633, 1, {1, "W", "Inverter L1 Power"});
    ModBus_read(modbus, 634, 1, {1, "W", "Inverter L2 Power"});
    ModBus_read(modbus, 635, 1, {1, "W", "Inverter L3 Power"});
    ModBus_read(modbus, 540, 1, {0.1, "C", "DC Temperature"});
    ModBus_read(modbus, 541, 1, {0.1, "C", "AC Temperature"});
    ModBus_read(modbus, 514, 1, {0.1, "kWh", "Daily Battery Charge"});
    ModBus_read(modbus, 515, 1, {0.1, "kWh", "Daily Battery Discharge"});
    ModBus_read(modbus, 516, 1, {0.1, "kWh", "Total Battery Charge"});
    ModBus_read(modbus, 517, 1, {0.1, "kWh", "Total Battery Charge"});
    ModBus_read(modbus, 518, 1, {0.1, "kWh", "Total Battery Discharge"});
    ModBus_read(modbus, 519, 1, {0.1, "kWh", "Total Battery Discharge"});
    ModBus_read(modbus, 590, 1, {1, "W", "Battery Power"});
    ModBus_read(modbus, 587, 1, {0.01, "V", "Battery Voltage"});
    ModBus_read(modbus, 588, 1, {1, "%", "Battery SOC"});
    ModBus_read(modbus, 591, 1, {0.01, "A", "Battery Current"});
    ModBus_read(modbus, 586, 1, {0.1, "C", "Battery Temperature"});
    ModBus_read(modbus, 653, 1, {1, "W", "Total Load Power"});
    ModBus_read(modbus, 650, 1, {1, "W", "Load L1 Power"});
    ModBus_read(modbus, 651, 1, {1, "W", "Load L2 Power"});
    ModBus_read(modbus, 652, 1, {1, "W", "Load L3 Power"});
    ModBus_read(modbus, 644, 1, {0.1, "V", "Load Voltage L1"});
    ModBus_read(modbus, 645, 1, {0.1, "V", "Load Voltage L2"});
    ModBus_read(modbus, 646, 1, {0.1, "V", "Load Voltage L3"});
    ModBus_read(modbus, 526, 1, {0.1, "kWh", "Daily Load Consumption"});
    ModBus_read(modbus, 527, 1, {0.1, "kWh", "Total Load Consumption"});
    ModBus_read(modbus, 528, 1, {0.1, "kWh", "Total Load Consumption"});
    ModBus_read(modbus, 588, 1, {1, "%", "Battery Capacity"});
}

int main(int argc, char**argv){
    QCoreApplication app(argc, argv);
    
    QCommandLineParser parser;
    parser.setApplicationDescription("Deye Modbus Client");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption verboseOption("verbose", "Verbose mode");
    parser.addOption(verboseOption);

    QCommandLineOption loopOption("loop", "Read report in loop");
    parser.addOption(loopOption);

    QCommandLineOption portsOption("ports", "List serial ports");
    parser.addOption(portsOption);

    parser.addOption({{"d", "device"}, "Open serial port <device>.", "device"});
    parser.addOption({{"p", "parity"}, "Set parity (none, even, odd, mark, space).", "parity"});
    parser.addOption({{"b", "baud"}, "Set baud rate (e.g., 9600, 115200).", "baud"});
    parser.addOption({{"l", "dataBits"}, "Set number of data bits (5, 6, 7, or 8).", "dataBits"});
    parser.addOption({{"s", "stopBits"}, "Set number of stop bits (1 or 2).", "stopBits"});
    parser.addOption({{"t", "responseTime"}, "Set response timeout in milliseconds.", "responseTime"}); 
    parser.addOption({{"r", "numberOfRetries"}, "Set number of retries for communication errors.", "numberOfRetries"});
    parser.addOption({{"i", "interval"}, "Loop <interval>.", "interval"});

    parser.process(app);

    if (parser.isSet(verboseOption)){
        QLoggingCategory::setFilterRules("qt.modbus* = true");
    }
    
    if (parser.isSet(portsOption)){
        const auto serialPortInfos = QSerialPortInfo::availablePorts();
        for (const QSerialPortInfo &portInfo : serialPortInfos) {
            qDebug() << "\n"
                    << "Port:" << portInfo.portName() << "\n"
                    << "Location:" << portInfo.systemLocation() << "\n"
                    << "Description:" << portInfo.description() << "\n"
                    << "Manufacturer:" << portInfo.manufacturer() << "\n"
                    << "Serial number:" << portInfo.serialNumber() << "\n"
                    << "Vendor Identifier:"
                    << (portInfo.hasVendorIdentifier()
                        ? QByteArray::number(portInfo.vendorIdentifier(), 16)
                        : QByteArray()) << "\n"
                    << "Product Identifier:"
                    << (portInfo.hasProductIdentifier()
                        ? QByteArray::number(portInfo.productIdentifier(), 16)
                        : QByteArray());
        }

        return 0;
    }

    const auto device = parser.value("device");
    const auto parity = parser.value("parity");
    const auto baud = parser.value("baud");
    const auto dataBits = parser.value("dataBits");
    const auto stopBits = parser.value("stopBits");
    const auto responseTime = parser.value("responseTime"); 
    const auto numberOfRetries = parser.value("numberOfRetries");

    if (parser.isSet(verboseOption)){
        qDebug() << "Device:" << device;
        qDebug() << "Parity:" << parity;
        qDebug() << "Baud rate:" << baud;
        qDebug() << "Data bits:" << dataBits;
        qDebug() << "Stop bits:" << stopBits;
        qDebug() << "Response time (ms):" << responseTime;
        qDebug() << "Number of retries:" << numberOfRetries;
    }

    auto settings = ModBusSettings(
        device, 
        ModBus_parity_from_string(parity), 
        baud.toInt(), 
        dataBits.toInt(), 
        stopBits.toInt(), 
        responseTime.toInt(), 
        numberOfRetries.toInt()
    );

    if (parser.isSet(verboseOption)){
        qDebug() << settings.toString();
    }

    auto modbus = ModBus_create(settings);

    if(modbus == nullptr){
        return 1;
    }
    
    if (parser.isSet(verboseOption)){
        qDebug() << modbus;
    }

     if (!modbus->connectDevice()) {
        qDebug() << QString("Connect failed: %1").arg(modbus->errorString());
        return 2;
    }

    if(parser.isSet(loopOption) == false){
        ModBus_read_report(modbus);
        QTimer::singleShot(5000, &QCoreApplication::quit);
        return app.exec();
    }

    auto interval = parser.value("interval").toInt();
    if(interval == 0){
        interval = 5000;
    }

    QTimer timer;
    timer.setInterval(interval);
    
    QObject::connect(&timer, &QTimer::timeout, [modbus](){
        qDebug() << "Report:" << QDateTime::currentDateTime().toString();
        ModBus_read_report(modbus);
    });

    timer.start();

    return app.exec();
}
