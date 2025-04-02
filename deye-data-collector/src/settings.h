#ifndef SETTINGS_H
#define SETTINGS_H

#include <QCommandLineParser>
#include <QJsonObject>
#include <QSharedData>
#include <QSharedDataPointer>

struct InverterSettings {
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
};

struct SettingsPrivate : public QSharedData {
    QList<InverterSettings> m_inverters;
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
        /*
        if(hasValue(source, "device") == true){
            setDevice(convert(source.value("device")));
        }

        if(hasValue(source, "parity") == true){
            setParity(convert(source.value("parity")));
        }

        if(hasValue(source, "instance") == true){
            setInstance(convert(source.value("instance")));
        }

        if(hasValue(source, "baud") == true){
            setBaud(source.value("baud").toInt());
        }

        if(hasValue(source, "data_bits") == true){
            setDataBits(source.value("data_bits").toInt());
        }

        if(hasValue(source, "stop_bits") == true){
            setStopBits(source.value("stop_bits").toInt());
        }

        if(hasValue(source, "response_time") == true){
            setResponseTime(source.value("response_time").toInt());
        }

        if(hasValue(source, "number_of_retries") == true){ 
           setNumberOfRetries(source.value("number_of_retries").toInt());
        }
        */

        if(hasValue(source, "listen") == true){
            setListen(source.value("listen").toInt());
        }

        if(hasValue(source, "interval") == true){
            setInterval(source.value("interval").toInt());
        }

        //if (hasValue(source, "driver") == true) {
        //    setDriver(convert(source.value("driver")));
        //}

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
        return QString(
            "Settings(\n"
            "  listen: %1,\n"
            "  interval: %2,\n"
            "  http_server: %3,\n"
            "  mqtt_client: %4,\n"
            "  mqtt_host: %5,\n"
            "  mqtt_port: %6,\n"
            "  mqtt_user: %7,\n"
            "  mqtt_password: [hidden],\n"  // Masked for security
            "  verbosity: %8,\n"
            "  loop: %9,\n"
            "  ports: %10\n"
            ")"
            )
            .arg(m_d->m_listen)
            .arg(m_d->m_interval)
            .arg(m_d->m_httpserver ? "true" : "false")  // Convert bool to string
            .arg(m_d->m_mqttclient ? "true" : "false")
            .arg(m_d->m_mqttHost)
            .arg(m_d->m_mqttPort)
            .arg(m_d->m_mqttUser)
            // Skip password in output for security
            .arg(m_d->m_verbosity)
            .arg(m_d->m_loop ? "true" : "false")
            .arg(m_d->m_ports ? "true" : "false");
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
