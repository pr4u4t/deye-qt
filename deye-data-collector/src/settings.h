#ifndef SETTINGS_H
#define SETTINGS_H

#include <QCommandLineParser>
#include <QJsonObject>
#include <QJsonArray>
#include <QSharedData>
#include <QSharedDataPointer>

struct InverterSettings {
    InverterSettings(const QJsonObject& inv) {
        if (inv.contains("device")) {
			m_device = inv.value("device").toString();
        }

        if (inv.contains("parity")) {
			m_parity = inv.value("parity").toString();
        }

        if (inv.contains("instance")) {
			m_instance = inv.value("instance").toString();
        }

        if (inv.contains("baud")) {
			m_baud = inv.value("baud").toInt();
        }

        if (inv.contains("data_bits")) {
			m_dataBits = inv.value("data_bits").toInt();
        }

        if (inv.contains("stop_bits")) {
			m_stopBits = inv.value("stop_bits").toInt();
        }

        if (inv.contains("response_time")) {
			m_responseTime = inv.value("response_time").toInt();
        }

        if (inv.contains("number_of_retries")) {
			m_numberOfRetries = inv.value("number_of_retries").toInt();
        }

        if (inv.contains("driver")) {
			m_driver = inv.value("driver").toString();
        }
    }

    QString m_device = "/dev/ttyUSB0";
    QString m_parity = "none";
    QString m_instance = "Instance 1";
    int m_baud = 9600;
    int m_dataBits = 8;
    int m_stopBits = 1;
    int m_responseTime = 1000;
    int m_numberOfRetries = 3;
    QString m_driver = "Deye";

    QString device() const { return m_device; }
    QString parity() const { return m_parity; }
    QString instance() const { return m_instance; }
    int baud() const { return m_baud; }
    int dataBits() const { return m_dataBits; }
    int stopBits() const { return m_stopBits; }
    int responseTime() const { return m_responseTime; }
    int numberOfRetries() const { return m_numberOfRetries; }
    QString driver() const { return m_driver; }

    void setDevice(const QString& device) { m_device = device; }
    void setParity(const QString& parity) { m_parity = parity; }
    void setInstance(const QString& instance) { m_instance = instance; }
    void setBaud(int baud) { m_baud = baud; }
    void setDataBits(int dataBits) { m_dataBits = dataBits; }
    void setStopBits(int stopBits) { m_stopBits = stopBits; }
    void setResponseTime(int responseTime) { m_responseTime = responseTime; }
    void setNumberOfRetries(int numberOfRetries) { m_numberOfRetries = numberOfRetries; }
    void setDriver(const QString& driver) { m_driver = driver; }

    // toString method to convert the struct to a readable string
    QString toString() const {
        return QString("InverterSettings {\n"
            "  instance: %1\n"
            "  device: %2\n"
            "  driver: %3\n"
            "  baud: %4\n"
            "  parity: %5\n"
            "  dataBits: %6\n"
            "  stopBits: %7\n"
            "  responseTime: %8\n"
            "  numberOfRetries: %9\n"
            "}")
            .arg(m_instance)
            .arg(m_device)
            .arg(m_driver)
            .arg(m_baud)
            .arg(m_parity)
            .arg(m_dataBits)
            .arg(m_stopBits)
            .arg(m_responseTime)
            .arg(m_numberOfRetries);
    }

    // Convert to QJsonObject
    QJsonObject toJsonObject() const {
        QJsonObject obj;
        obj["instance"] = m_instance;
        obj["device"] = m_device;
        obj["driver"] = m_driver;
        obj["baud"] = m_baud;
        obj["parity"] = m_parity;
        obj["data_bits"] = m_dataBits;
        obj["stop_bits"] = m_stopBits;
        obj["response_time"] = m_responseTime;
        obj["number_of_retries"] = m_numberOfRetries;
        return obj;
    }

    // QDebug output support
    friend QDebug operator<<(QDebug debug, const InverterSettings& settings) {
        debug.nospace().noquote() << settings.toString();
        return debug;
    }
};

struct SettingsPrivate : public QSharedData {
    QVector<InverterSettings> m_inverters;
    int m_listen = 8080;
    int m_interval = 5000;
    bool m_httpserver = false;
    bool m_mqttclient = false;
    QString m_mqttHost = "core-mosquitto";
    qint16 m_mqttPort = 1883;
    QString m_mqttUser = "homeassistant";
    QString m_mqttPassword = "password";
    int m_verbosity = 1;
    bool m_loop = false;
    bool m_ports = false;

    QString toString() const {
        QString result = QString("SettingsPrivate {\n");

        // Add inverters information
        result += QString("  Inverters (%1):\n").arg(m_inverters.size());
        for (int i = 0; i < m_inverters.size(); ++i) {
            QString inverterStr = m_inverters[i].toString();
            // Indent each line of the inverter string with two additional spaces
            inverterStr.replace("\n", "\n    ");
            result += QString("    Inverter %1: %2\n").arg(i + 1).arg(inverterStr);
        }

        // Add other settings
        result += QString("  HTTP Server: %1\n").arg(m_httpserver ? "enabled" : "disabled");
        result += QString("  HTTP Port: %1\n").arg(m_listen);
        result += QString("  Polling Interval: %1 ms\n").arg(m_interval);
        result += QString("  MQTT Client: %1\n").arg(m_mqttclient ? "enabled" : "disabled");
        result += QString("  MQTT Host: %1\n").arg(m_mqttHost);
        result += QString("  MQTT Port: %1\n").arg(m_mqttPort);
        result += QString("  MQTT User: %1\n").arg(m_mqttUser);
        result += QString("  MQTT Password: %1\n").arg(m_mqttPassword.isEmpty() ? "[not set]" : "********");
        result += QString("  Verbosity Level: %1\n").arg(m_verbosity);
        result += QString("  Loop Mode: %1\n").arg(m_loop ? "enabled" : "disabled");
        result += QString("  Show Ports: %1\n").arg(m_ports ? "enabled" : "disabled");

        result += "}";
        return result;
    }

    // QDebug output support
    friend QDebug operator<<(QDebug debug, const SettingsPrivate& settings) {
        debug.nospace().noquote() << settings.toString();
        return debug;
    }
};


class Settings{
public:
    Settings() 
        : m_d(new SettingsPrivate()){
    }

    Settings(const Settings& other)
        : m_d(other.m_d) {
    }

    //Settings(const QCommandLineParser& parser)
    //    : m_d(new SettingsPrivate()){
    //    fillFromCmd(parser);
    //}

    Settings(const QJsonObject& object)
        : m_d(new SettingsPrivate()){
        fillFromJson(object);
    }

    void fillFromJson(const QJsonObject& object){
        fill<QJsonObject>(object, [](const QJsonObject& source, const QString& key){
            return source.contains(key) == true;
        });
    }

    const QVector<InverterSettings>& inverters() const { return m_d->m_inverters; }

    //void fillFromCmd(const QCommandLineParser& parser){
    //    fill<QCommandLineParser>(parser, [](const QCommandLineParser& source, const QString& key){
    //        return source.isSet(key) == true;
    //    });
    //}

    template<typename T>
    QString convert(const T& value) const{
        return value.toString();
    }

    QString convert(const QString& value) const{
        return value;
    }

    bool toBool(const QString& str) const {
        return (str == "true") ? true : false;
    }

    bool toBool(const QJsonValue& val) const {
        return val.toBool();
    }

    template<typename T>
    void fill(const T& source, const std::function<bool (const T&, const QString&)>& hasValue){
        if (!source.contains("inverters") || !source["inverters"].isArray()) {
            qDebug() << "Object doesn't contain an array with key: inverters";
            return;
        }

        // Get the JSON array
        QJsonArray jsonArray = source["inverters"].toArray();

        for (int i = 0; i < jsonArray.size(); ++i) {
            // Process each element
            QJsonValue value = jsonArray[i];

            // Handle different value types
            if (value.isObject()) {
                QJsonObject obj = value.toObject();
                qDebug() << "Object at index" << i << ":" << obj;
				m_d->m_inverters.push_back(InverterSettings(obj));
            } else {
				qWarning() << "Element at index" << i << "is not an object.";
            }
        }

        if(hasValue(source, "listen") == true){
            setListen(source.value("listen").toInt());
        }

        if(hasValue(source, "interval") == true){
            setInterval(source.value("interval").toInt());
        }

        if (hasValue(source, "verbosity") == true) {
            setVerbosity(source.value("verbosity").toInt());
        }

        if (hasValue(source, "ports") == true) {
            setPorts(toBool(source.value("ports")));
        }

        if (hasValue(source, "loop") == true) {
            setLoop(toBool(source.value("loop")));
        }

        if (hasValue(source, "http_server") == true) {
            setHttpServer(toBool(source.value("http_server")));
        }

        if (hasValue(source, "mqtt_client") == true) {
            setMqttClient(toBool(source.value("mqtt_client")));
        }

		if (hasValue(source, "mqtt_host") == true) {
			setMqttHost(convert(source.value("mqtt_host")));
		}

        if (hasValue(source, "mqtt_user") == true) {
            setMqttUser(convert(source.value("mqtt_user")));
        }

		if (hasValue(source, "mqtt_password") == true) {
			setMqttPassword(convert(source.value("mqtt_password")));
		}
    }

    int listen() const { return m_d->m_listen; }
    int interval() const { return m_d->m_interval; }
    bool httpserver() const { return m_d->m_httpserver; }
    bool mqttclient() const { return m_d->m_mqttclient; }
    QString mqttHost() const { return m_d->m_mqttHost; }
    qint16 mqttPort() const { return m_d->m_mqttPort; }
    QString mqttUser() const { return m_d->m_mqttUser; }
    QString mqttPassword() const { return m_d->m_mqttPassword; }
    int verbosity() const { return m_d->m_verbosity;  }
    bool loop() const { return m_d->m_loop;  }
    bool ports() const { return m_d->m_ports; }

    
    void setListen(int listen) { m_d->m_listen = listen; }
    void setInterval(int interval) { m_d->m_interval = interval; }
    void setHttpServer(bool httpserver) { m_d->m_httpserver = httpserver; }
    void setMqttClient(bool mqttclient) { m_d->m_mqttclient = mqttclient; }
    void setMqttHost(const QString& mqttHost) { m_d->m_mqttHost = mqttHost; }
    
    void setVerbosity(int verbosity) { m_d->m_verbosity = verbosity;  }
    void setLoop(bool loop) { m_d->m_loop = loop; }
    void setMqttPort(qint16 mqttPort) { m_d->m_mqttPort = mqttPort; }
    void setMqttUser(const QString& mqttUser) { m_d->m_mqttUser = mqttUser; }
    void setMqttPassword(const QString& mqttPassword) { m_d->m_mqttPassword = mqttPassword; }
    void setPorts(bool ports) { m_d->m_ports = ports; }

    QString toString() const {
        return m_d->toString();
    }

    // QDebug output support
    friend QDebug operator<<(QDebug debug, const Settings &settings) {
        debug.nospace().noquote() << settings.toString();
        return debug;
    }

private:
   QSharedDataPointer<SettingsPrivate> m_d;
};

#endif
