#include <QVariant>
#include <QTimer>
#include <QDateTime>

#include "main.h"
#include "utils.h"

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
        SerialPort_parity_from_string(parity), 
        baud.toInt(), 
        dataBits.toInt(), 
        stopBits.toInt(), 
        responseTime.toInt(), 
        numberOfRetries.toInt()
    );

    if (parser.isSet(verboseOption)){
        qDebug() << settings.toString();
    }

    auto deye = new Deye(settings);

    if(deye == nullptr){
        return 1;
    }
    
    if (parser.isSet(verboseOption)){
        qDebug() << deye;
    }

    if (!deye->connectDevice()) {
        return 2;
    }

    if(parser.isSet(loopOption) == false){
        deye->readReport();
        QTimer::singleShot(5000, &QCoreApplication::quit);
        return app.exec();
    }

    auto interval = parser.value("interval").toInt();
    if(interval == 0){
        interval = 5000;
    }

    QTimer timer;
    timer.setInterval(interval);
    
    QObject::connect(&timer, &QTimer::timeout, [deye](){
        qDebug() << "Report:" << QDateTime::currentDateTime().toString();
        deye->readReport();
    });

    timer.start();

    return app.exec();
}
