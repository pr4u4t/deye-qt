#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QMqttClient>
#include <QJsonDocument>
#include <QJsonObject>

#include "inverter.h"
#include "settings.h"
#include "output.h"

class MqttClient : public QObject{
    Q_OBJECT
public:

    MqttClient(const Settings& settings, QObject* parent = nullptr)
        : QObject(parent)
        , m_client(new QMqttClient(this)){
        m_client->setHostname(settings.mqttHost());
        m_client->setPort(settings.mqttPort());
        m_client->setUsername(settings.mqttUser());
        m_client->setPassword(settings.mqttPassword());
    }

    ~MqttClient() override = default;

    bool init(){
        if(m_client == nullptr){
            qDebug() << "Failed to create mqtt client";
            return false;
        }

        return true;
    }

    bool deinit(){
        if(m_client){
            m_client->disconnectFromHost();
            delete m_client;
            m_client = nullptr;
        }

        return true;
    }

private:
    void publishAutoDiscovery(const QVector<Sensor>& sensors) {
    
    for (const auto &sensor : sensors) {
        QJsonObject payload {
            {"name", "Deye " + sensor.name()},
            {"state_topic", "deye/sensor/" + sensor.topicSuffix() + "/state"},
            {"unit_of_measurement", sensor.unit()},
            {"device_class", sensor.deviceClass()},
            {"unique_id", sensor.uniqueId()},
            {"availability_topic", "deye/status"}
        };

        m_client->publish(
            QMqttTopicName("homeassistant/sensor/" + sensor.uniqueId() + "/config"),
            QJsonDocument(payload).toJson(),
            1,  // QoS 1
            true // Retain
        );
    }
    
    // Publish online status
    m_client->publish(QMqttTopicName("deye/status"), "online", 1, true);
}

private:
    QMqttClient* m_client = nullptr;
    Settings m_settings;
};

#endif // MQTTCLIENT_H