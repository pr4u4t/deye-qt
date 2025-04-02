#include <QVariant>
#include <QTimer>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QMqttClient>
#include <QDir>
#include <QFileInfo>

#include "main.h"
#include "utils.h"
#include "output.h"
#include "httpserver.h"
#include "mqttclient.h"

bool config_load(const QString &path, QJsonObject &config) {
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

void serial_port_dump(){
    qDebug() << "void serial_port_dump()";
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
    parser.addOption({ {"o", "loop"}, "Whether to execute reading in loop <true|false>", "loop" });
    parser.addOption({ {"k", "ports"}, "Whether to list local modbus ports <true|false>", "ports" });
    parser.addOption({ {"z", "mqtt_host"}, "MQTT broker hostname or IP address", "mqtt_host" });
    parser.addOption({ {"u", "mqtt_user"}, "Username for MQTT authentication", "mqtt_user" });
    parser.addOption({ {"w", "mqtt_password"}, "Password for MQTT authentication", "mqtt_password" });
}

void load_settings(Settings& settings, const QCommandLineParser& parser) {
    qDebug() << "void load_settings(Settings& settings, const QCommandLineParser& parser)";
    const auto conf = parser.value("config");

    if (conf.size() > 0) {
        qInfo() << "Reading configuration from file";
        QJsonObject cnf;
        if (config_load(conf, cnf) == true) {
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

void single_run(Inverter* inv, const Settings& settings, QTimer& timer, QVector<Output*>& outputs) {
    qDebug() << "void single_run(Inverter* inv, const Settings& settings)";
    Q_UNUSED(settings)
    Q_UNUSED(timer)
    Q_UNUSED(outputs)

    qDebug() << "Performing single read";

    QObject::connect(inv, &Inverter::reportReady, [inv](const QJsonObject& report) {
        qInfo().noquote() << "report:" << QString(QJsonDocument(report).toJson());

        QTimer::singleShot(0,QCoreApplication::instance(), QCoreApplication::quit);
    });

    inv->readReport();
}

void loop_run(Inverter* inv, const Settings& settings, QTimer& timer, QVector<Output*>& outputs) {
    qDebug() << "void loop_run(Inverter* inv, const Settings& settings)";

    if (settings.httpserver()) {
        auto server = new HttpServer(settings);
        if (server->init() == false) {
			qCritical() << "Failed to start http server";
			return;
        }
		outputs.push_back(server);
        QObject::connect(inv, &Inverter::reportReady, server, &HttpServer::update);
    }

    if (settings.mqttclient()) {
		auto mqtt = new MqttClient(settings);
        if (mqtt->init() == false) {
			qCritical() << "Failed to start mqtt client";
            return;
        }
    
        outputs.push_back(mqtt);
        QObject::connect(inv, &Inverter::reportReady, mqtt, &MqttClient::update);
    }
 
    timer.setInterval(settings.interval());

    QObject::connect(&timer, &QTimer::timeout, [inv](){
        qDebug() << "Report:" << QDateTime::currentDateTime().toString();
        inv->readReport();
    });

    timer.start();
}

int main(int argc, char** argv){
    Settings config;
    QCommandLineParser parser;
    QTimer timer;
    QVector<Output*> outputs;

    qDebug() << "***************************************************************";
    qDebug() << "*                  STARTING INVERTER                          *";
    qDebug() << "***************************************************************";

    QCoreApplication app(argc, argv);

    debug_arguments(argc, argv);

    qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz t} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif}] %{file}:%{line} - %{message}");

    setup_parser(parser);

    parser.process(app);

    if (config.ports() == true) {
        serial_port_dump();
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
        single_run(inv, config, timer, outputs);
    } else {
        loop_run(inv, config, timer, outputs);
    }

    return app.exec();
}
