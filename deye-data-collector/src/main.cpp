#include <QVariant>
#include <QTimer>
#include <QDateTime>
#include <QJsonObject>
#include <QHttpServer>
#include <QJsonDocument>
#include <QTcpServer>
#include <QFile>
#include <QMqttClient>

#include "main.h"
#include "utils.h"

bool HttpServer_start(const Settings& settings, QHttpServer* server, QJsonObject* data){
    // Define a route for "/sensor"
    server->route("/sensor", QHttpServerRequest::Method::Get, [data] (const QHttpServerRequest &request, QHttpServerResponder &responder) {
        Q_UNUSED(request);
        responder.write(QJsonDocument(*data).toJson(), "application/json");
    });

    // Start the server on port 8080
    auto tcpserver = new QTcpServer();
    if (!tcpserver->listen(QHostAddress::Any, settings.listen) || !server->bind(tcpserver)) {
        delete tcpserver;
        qCritical() << "Failed to start server!";
        return false;
    }

    return true;
}

bool Config_load(const QString &path, QJsonObject &config) {
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

void Mqtt_publishAutoDiscovery(QMqttClient* mqttClient, const QVector<DeyeSensor>& sensors) {
    
    for (const auto &sensor : sensors) {
        QJsonObject payload {
            {"name", "Deye " + sensor.name},
            {"state_topic", "deye/sensor/" + sensor.topicSuffix + "/state"},
            {"unit_of_measurement", sensor.unit},
            {"device_class", sensor.deviceClass},
            {"unique_id", sensor.uniqueId},
            {"availability_topic", "deye/status"}
        };

        mqttClient->publish(
            QMqttTopicName("homeassistant/sensor/" + sensor.uniqueId + "/config"),
            QJsonDocument(payload).toJson(),
            1,  // QoS 1
            true // Retain
        );
    }
    
    // Publish online status
    mqttClient->publish(QMqttTopicName("deye/status"), "online", 1, true);
}

QMqttClient* Mqtt_clientSetup(const QString& id, const QVector<DeyeSensor>& sensors) {
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

int main(int argc, char**argv){
    Settings config;
    QCommandLineParser parser;
    QJsonObject model;
    QTimer timer;
    QHttpServer server;

    qDebug() << "***************************************************************";
    qDebug() << "*                  STARTING INVERTER                          *";
    qDebug() << "***************************************************************";

    QCoreApplication app(argc, argv);
    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz t} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");

    parser.setApplicationDescription("Deye Modbus Client");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOption({{"e", "verbosity"}, "Set verbosity level (0-3).", "verbosity"});

    QCommandLineOption loopOption("loop", "Read report in loop");
    parser.addOption(loopOption);

    QCommandLineOption portsOption("ports", "List serial ports");
    parser.addOption(portsOption);

    parser.addOption({{"d", "device"}, "Open serial port <device>.", "device"});
    parser.addOption({{"p", "parity"}, "Set parity (none, even, odd, mark, space).", "parity"});
    parser.addOption({{"b", "baud"}, "Set baud rate (e.g., 9600, 115200).", "baud"});
    parser.addOption({{"l", "data_bits"}, "Set number of data bits (5, 6, 7, or 8).", "dataBits"});
    parser.addOption({{"s", "stop_bits"}, "Set number of stop bits (1 or 2).", "stopBits"});
    parser.addOption({{"t", "response_time"}, "Set response timeout in milliseconds.", "responseTime"}); 
    parser.addOption({{"r", "number_of_retries"}, "Set number of retries for communication errors.", "numberOfRetries"});
    parser.addOption({{"i", "interval"}, "Loop <interval>.", "interval"});
    parser.addOption({{"c", "config"}, "Configuration file path", "config"});
    parser.addOption({{"n", "listen"}, "Listen port number", "listen"});
    parser.addOption({{"a", "instance"}, "Instance name", "instance"});
    parser.addOption({{"m", "http_server"}, "Whether to start http server or not <true|false>", "httpserver"});
    parser.process(app);

    if (parser.isSet("verbosity")) {
        int level = parser.value("verbosity").toInt();
        switch(level) {
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
    
    if (parser.isSet(portsOption)){
        SerialPort_dump();
        return 0;
    }
    
    const auto conf = parser.value("config");
    qDebug() << "Using configuration file: " << conf;
    if(conf.size() > 0){
        qInfo() << "Reading configuration from file";
        QJsonObject cnf;
        if(Config_load(conf, cnf) == true){
            config.fillFromJson(cnf);
            qDebug() << "Configuration from file: " << config;
        } else {
            qDebug() << "Configuration file not found: " << config;
        }
    }

    config.fillFromCmd(parser);
    qDebug() << "Configuration after cmd: " << config;
    
    auto deye = new Deye(config, &model);
    auto mqtt = Mqtt_clientSetup(config.device, deye->sensors());
    deye->setMqtt(mqtt);

    if(deye == nullptr){
        qCritical() << "Failed to create deye instance... quitting";
        return 1;
    }

    if (!deye->connectDevice()) {
        qCritical() << "Failed to connect to inverter... quitting";
        return 2;
    }

    if(parser.isSet(loopOption) == false){
        qDebug() << "Performing single read";
        deye->readReport();
        QTimer::singleShot(5000, &QCoreApplication::quit);
        return app.exec();
    }

    qDebug() << "Starting loop mode";
    timer.setInterval(config.interval);

    QObject::connect(&timer, &QTimer::timeout, [deye](){
        qDebug() << "Report:" << QDateTime::currentDateTime().toString();
        deye->readReport();
    });

    timer.start();

    if(config.httpserver){
        qDebug() << "Starting http server";
        HttpServer_start(config, &server, &model);
    }

    return app.exec();
}
