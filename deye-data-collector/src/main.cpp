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

QVector<DeyeSensor> createSensorList() {
    return {
        // Temperature Sensors (scaling /10)
        {"AC Temperature", "°C", "temperature", "ac_temp", "deye_ac_temp_001", 0.1},
        {"Battery Temperature", "°C", "temperature", "battery_temp", "deye_battery_temp_002", 0.1},
        {"DC Temperature", "°C", "temperature", "dc_temp", "deye_dc_temp_003", 0.1},

        // Battery Sensors
        {"Battery Capacity", "%", "battery", "battery_capacity", "deye_battery_capacity_004", 1.0},
        {"Battery SOC", "%", "battery", "battery_soc", "deye_battery_soc_005", 1.0},
        {"Battery Current", "mA", "current", "battery_current", "deye_battery_current_006", 1.0},
        {"Battery Power", "W", "power", "battery_power", "deye_battery_power_007", 1.0},
        {"Battery Voltage", "V", "voltage", "battery_voltage", "deye_battery_voltage_008", 0.1},

        // Current Sensors (scaling /10)
        {"Current L1", "A", "current", "current_l1", "deye_current_l1_009", 0.1},
        {"Current L2", "A", "current", "current_l2", "deye_current_l2_010", 0.1},
        {"Current L3", "A", "current", "current_l3", "deye_current_l3_011", 0.1},

        // Voltage Sensors (scaling /10)
        {"Grid Voltage L1", "V", "voltage", "grid_voltage_l1", "deye_grid_voltage_l1_012", 0.1},
        {"Grid Voltage L2", "V", "voltage", "grid_voltage_l2", "deye_grid_voltage_l2_013", 0.1},
        {"Grid Voltage L3", "V", "voltage", "grid_voltage_l3", "deye_grid_voltage_l3_014", 0.1},
        {"Load Voltage L1", "V", "voltage", "load_voltage_l1", "deye_load_voltage_l1_015", 0.1},
        {"Load Voltage L2", "V", "voltage", "load_voltage_l2", "deye_load_voltage_l2_016", 0.1},
        {"Load Voltage L3", "V", "voltage", "load_voltage_l3", "deye_load_voltage_l3_017", 0.1},
        {"PV1 Voltage", "V", "voltage", "pv1_voltage", "deye_pv1_voltage_018", 0.1},
        {"PV2 Voltage", "V", "voltage", "pv2_voltage", "deye_pv2_voltage_019", 0.1},

        // Current Sensors (scaling /10)
        {"PV1 Current", "A", "current", "pv1_current", "deye_pv1_current_020", 0.1},
        {"PV2 Current", "A", "current", "pv2_current", "deye_pv2_current_021", 0.1},

        // Power Sensors
        {"PV1 Input Power", "W", "power", "pv1_input_power", "deye_pv1_input_power_022", 1.0},
        {"PV2 Power", "W", "power", "pv2_power", "deye_pv2_power_023", 1.0},
        {"Day Load Power", "W", "power", "day_load_power", "deye_day_load_power_024", 1.0},
        {"External CT L1 Power", "W", "power", "external_ct_l1_power", "deye_external_ct_l1_power_025", 1.0},
        {"External CT L2 Power", "W", "power", "external_ct_l2_power", "deye_external_ct_l2_power_026", 1.0},
        {"External CT L3 Power", "W", "power", "external_ct_l3_power", "deye_external_ct_l3_power_027", 1.0},
        {"Gen Port Power", "W", "power", "gen_port_power", "deye_gen_port_power_028", 1.0},
        {"GridSell Power", "W", "power", "gridsell_power", "deye_gridsell_power_029", 1.0},
        {"Internal CT L1 Power", "W", "power", "internal_ct_l1_power", "deye_internal_ct_l1_power_030", 1.0},
        {"Internal CT L2 Power", "W", "power", "internal_ct_l2_power", "deye_internal_ct_l2_power_031", 1.0},
        {"Internal CT L3 Power", "W", "power", "internal_ct_l3_power", "deye_internal_ct_l3_power_032", 1.0},
        {"Inverter L1 Power", "W", "power", "inverter_l1_power", "deye_inverter_l1_power_033", 1.0},
        {"Inverter L2 Power", "W", "power", "inverter_l2_power", "deye_inverter_l2_power_034", 1.0},
        {"Inverter L3 Power", "W", "power", "inverter_l3_power", "deye_inverter_l3_power_035", 1.0},
        {"Load L1 Power", "W", "power", "load_l1_power", "deye_load_l1_power_036", 1.0},
        {"Load L2 Power", "W", "power", "load_l2_power", "deye_load_l2_power_037", 1.0},
        {"Load L3 Power", "W", "power", "load_l3_power", "deye_load_l3_power_038", 1.0},
        {"Total Grid Power", "W", "power", "total_grid_power", "deye_total_grid_power_039", 1.0},
        {"Total Load Power", "W", "power", "total_load_power", "deye_total_load_power_040", 1.0},
        {"Total Active IN Power", "W", "power", "total_active_in_power", "deye_total_active_in_power_041", 1.0},

        // Energy Sensors (scaling /100)
        {"Daily Battery Charge", "kWh", "energy", "daily_battery_charge", "deye_daily_battery_charge_042", 0.01},
        {"Daily Battery Discharge", "kWh", "energy", "daily_battery_discharge", "deye_daily_battery_discharge_043", 0.01},
        {"Daily Energy Bought", "kWh", "energy", "daily_energy_bought", "deye_daily_energy_bought_044", 0.01},
        {"Daily Energy Sold", "kWh", "energy", "daily_energy_sold", "deye_daily_energy_sold_045", 0.01},
        {"Daily Load Consumption", "kWh", "energy", "daily_load_consumption", "deye_daily_load_consumption_046", 0.01},
        {"Daily Production", "kWh", "energy", "daily_production", "deye_daily_production_047", 0.01},
        {"Total Battery Charge", "kWh", "energy", "total_battery_charge", "deye_total_battery_charge_048", 0.01},
        {"Total Battery Discharge", "kWh", "energy", "total_battery_discharge", "deye_total_battery_discharge_049", 0.01},
        {"Total Energy Bought", "kWh", "energy", "total_energy_bought", "deye_total_energy_bought_050", 0.01},
        {"Total Energy Sold", "kWh", "energy", "total_energy_sold", "deye_total_energy_sold_051", 0.01},
        {"Total Load Consumption", "kWh", "energy", "total_load_consumption", "deye_total_load_consumption_052", 0.01},
        {"Total Production", "kWh", "energy", "total_production", "deye_total_production_053", 0.01}
    };
}

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

bool loadConfig(const QString &path, QJsonObject &config) {
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
    return true;
}

void publishAutoDiscovery(QMqttClient* mqttClient) {
    const auto sensors = createSensorList();
    
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

QMqttClient* setupMqttClient(const QString& id) {
    auto client = new QMqttClient();
    client->setHostname("localhost");  // HA Mosquitto broker
    client->setPort(1883);
    client->setClientId("deye-inverter-addon-"+id);
    
    QObject::connect(client, &QMqttClient::connected, [client]() {
        qDebug() << "MQTT Connected!";
        publishAutoDiscovery(client);
    });
    
    QObject::connect(client, &QMqttClient::errorChanged, [](QMqttClient::ClientError error) {
        qCritical() << "MQTT Error:" << error;
    });
    
    client->connectToHost();
    return client;
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
    parser.addOption({{"c", "config"}, "Configuration file path", "config"});
    parser.addOption({{"n", "listen"}, "Listen port number", "listen"});
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

    const auto conf = parser.value("config");
    Settings config;

    if(conf.size() > 0){
        qInfo() << "Reading configuration from file";
        QJsonObject cnf;
        loadConfig(conf, cnf);
        config.fillFromJson(cnf);
    }
    
    
       
    config.fillFromCmd(parser);

    if (parser.isSet(verboseOption)){
        qDebug() << "Device:" << config.device;
        qDebug() << "Parity:" << config.parity;
        qDebug() << "Baud rate:" << config.baud;
        qDebug() << "Data bits:" << config.dataBits;
        qDebug() << "Stop bits:" << config.stopBits;
        qDebug() << "Response time (ms):" << config.responseTime;
        qDebug() << "Number of retries:" << config.numberOfRetries;
        qDebug() << "Listen: " << config.listen;
    }

    QJsonObject model;
    auto deye = new Deye(config, &model);

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

    //-------------------------

    QHttpServer server;
    HttpServer_start(config, &server, &model);

    auto mqtt = setupMqttClient(config.device);

    return app.exec();
}
