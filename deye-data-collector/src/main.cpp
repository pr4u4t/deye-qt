#include <QVariant>
#include <QTimer>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QMqttClient>

#include "main.h"
#include "utils.h"

bool Config_load(const QString &path, QJsonObject &config) {
    qDebug() << "bool Config_load(const QString &path, QJsonObject &config)";
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open config file:" << path;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Invalid JSON format in config file.";
        return false;
    }

    config = doc.object();
    qDebug() << "json settings: " << config;
    return true;
}

void SerialPort_dump(){
    qDebug() << "void SerialPort_dump()";
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
}

void Mqtt_publishAutoDiscovery(QMqttClient* mqttClient, const QVector<Sensor>& sensors) {
    
    for (const auto &sensor : sensors) {
        QJsonObject payload {
            {"name", "Deye " + sensor.name()},
            {"state_topic", "deye/sensor/" + sensor.topicSuffix() + "/state"},
            {"unit_of_measurement", sensor.unit()},
            {"device_class", sensor.deviceClass()},
            {"unique_id", sensor.uniqueId()},
            {"availability_topic", "deye/status"}
        };

        mqttClient->publish(
            QMqttTopicName("homeassistant/sensor/" + sensor.uniqueId() + "/config"),
            QJsonDocument(payload).toJson(),
            1,  // QoS 1
            true // Retain
        );
    }
    
    // Publish online status
    mqttClient->publish(QMqttTopicName("deye/status"), "online", 1, true);
}

QMqttClient* Mqtt_clientSetup(const QString& id, const QVector<Sensor>& sensors) {
    auto client = new QMqttClient();
    client->setHostname("core-mosquitto");  // HA Mosquitto broker
    client->setUsername("homeassistant");
    client->setPassword("YieVi1aeceoruavoo6io6uChaeD2looweil5aishooshoh7dan4ahreeYa1eal4o");
    client->setPort(1883);
    client->setClientId("deye-inverter-addon-"+id);
    client->setProtocolVersion(QMqttClient::MQTT_3_1_1);
    
    QObject::connect(client, &QMqttClient::connected, [client, sensors]() {
        qDebug() << "MQTT Connected!";
        Mqtt_publishAutoDiscovery(client, sensors);
    });
    
    QObject::connect(client, &QMqttClient::errorChanged, [](QMqttClient::ClientError error) {
        qCritical() << "MQTT Error:" << error;
    });
    
    client->connectToHost();
    return client;
}

void debug_arguments(int& argc, char** argv) {
    qDebug() << "<<<<<<<<<<< Arguments";
    for (int j = 0; j < argc; ++j) {
        qDebug() << "argument:" << argv[j];
    }
    qDebug() << ">>>>>>>>>>>";
}

Inverter* choose_driver(const Settings& settings) {
    qDebug() << "Inverter* choose_driver(const QString& driver, const Settings& settings)";
    if (settings.driver().toLower() == "deye") {
        return new Deye(settings);
    }

    if (settings.driver().toLower() == "dummy") {
        return new Dummy(settings);
    }

    return nullptr;
}

void setup_parser(QCommandLineParser& parser) {
    qDebug() << "void setup_parser(QCommandLineParser& parser)";
    parser.setApplicationDescription("Deye Modbus Client");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption({ {"e", "verbosity"}, "Set verbosity level (0-3).", "verbosity" });

    QCommandLineOption loopOption("loop", "Read report in loop");
    parser.addOption(loopOption);

    QCommandLineOption portsOption("ports", "List serial ports");
    parser.addOption(portsOption);

    parser.addOption({ {"d", "device"}, "Open serial port <device>.", "device" });
    parser.addOption({ {"p", "parity"}, "Set parity (none, even, odd, mark, space).", "parity" });
    parser.addOption({ {"b", "baud"}, "Set baud rate (e.g., 9600, 115200).", "baud" });
    parser.addOption({ {"l", "data_bits"}, "Set number of data bits (5, 6, 7, or 8).", "dataBits" });
    parser.addOption({ {"s", "stop_bits"}, "Set number of stop bits (1 or 2).", "stopBits" });
    parser.addOption({ {"t", "response_time"}, "Set response timeout in milliseconds.", "responseTime" });
    parser.addOption({ {"r", "number_of_retries"}, "Set number of retries for communication errors.", "numberOfRetries" });
    parser.addOption({ {"i", "interval"}, "Loop <interval>.", "interval" });
    parser.addOption({ {"c", "config"}, "Configuration file path", "config" });
    parser.addOption({ {"n", "listen"}, "Listen port number", "listen" });
    parser.addOption({ {"a", "instance"}, "Instance name", "instance" });
    parser.addOption({ {"m", "http_server"}, "Whether to start http server or not <true|false>", "httpserver" });
    parser.addOption({ {"q", "mqtt_client"}, "Whether to use mqtt client<true|false>", "mqttclient" });
    parser.addOption({ {"g", "driver"}, "Driver to use <Deye|Dummy>", "driver" });
}

void load_settings(Settings& settings, const QCommandLineParser& parser) {
    qDebug() << "void load_settings(Settings& settings, const QCommandLineParser& parser)";
    const auto conf = parser.value("config");

    if (conf.size() > 0) {
        qInfo() << "Reading configuration from file";
        QJsonObject cnf;
        if (Config_load(conf, cnf) == true) {
            settings.fillFromJson(cnf);
            qDebug() << "Configuration from file: " << settings;
        }
        else {
            qDebug() << "Configuration file not found: " << settings;
        }
    }
    else {
        qDebug() << "Configuration file path empty";
    }

    settings.fillFromCmd(parser);
    qDebug() << "Configuration after cmd: " << settings;
}

void set_logger_verbosity(const Settings& settings) {
    qDebug() << "void set_logger_verbosity(const Settings& settings)";
    switch (settings.verbosity()) {
        case 0: // Silent
            QLoggingCategory::setFilterRules("*.debug=false;*.warning=false");
            break;
        case 1: // Minimal
            QLoggingCategory::setFilterRules("*.debug=false;*.warning=true");
            break;
        case 2: // Medium
            QLoggingCategory::setFilterRules("*.debug=true;qt.*.debug=false");
            break;
        case 3: // Maximum
            QLoggingCategory::setFilterRules("*.debug=true");
            break;
    }
}

void single_run(Inverter* inv, const Settings& settings) {
    qDebug() << "void single_run(Inverter* inv, const Settings& settings)";
    Q_UNUSED(settings)
    qDebug() << "Performing single read";

    QObject::connect(inv, &Inverter::reportReady, [inv](const QJsonObject& report) {
        qInfo().noquote() << "report:" << QString(QJsonDocument(report).toJson());

        QTimer::singleShot(0,QCoreApplication::instance(), QCoreApplication::quit);
    });

    inv->readReport();
}

void loop_run(Inverter* inv, const Settings& settings) {
    qDebug() << "void loop_run(Inverter* inv, const Settings& settings)";
    /*
qDebug() << "Starting loop mode";
timer.setInterval(config.interval());

QObject::connect(&timer, &QTimer::timeout, [deye](){
    qDebug() << "Report:" << QDateTime::currentDateTime().toString();
    deye->readReport();
});

timer.start();

if(config.httpserver()){
    qDebug() << "Starting http server";
    //HttpServer_start(config, &server, &model);
}
*/
}

int main(int argc, char** argv){
    Settings config;
    QCommandLineParser parser;
    QTimer timer;

    qDebug() << "***************************************************************";
    qDebug() << "*                  STARTING INVERTER                          *";
    qDebug() << "***************************************************************";

    QCoreApplication app(argc, argv);

    debug_arguments(argc, argv);

    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz t} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");

    setup_parser(parser);

    parser.process(app);

    if (config.ports() == true) {
        SerialPort_dump();
        return 0;
    }

    load_settings(config, parser);

    set_logger_verbosity(config);
    
    auto inv = choose_driver(config);

    if(inv == nullptr){
        qCritical() << "Failed to create Inverter instance... quitting";
        return 1;
    }

    if (!inv->connectDevice() == true) {
        qCritical() << "Failed to connect to inverter... quitting";
        return 2;
    }

    if(config.loop() == false) {
        single_run(inv, config);
    } else {
        loop_run(inv, config);
    }

    return app.exec();
}
