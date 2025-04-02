#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QMqttClient>
#include <QJsonDocument>
#include <QJsonObject>

#include "inverter.h"
#include "settings.h"
#include "output.h"

class MqttClient : public QObject
                 , public Output{
    Q_OBJECT
public:

    MqttClient(const Settings& settings, QObject* parent = nullptr)
        : QObject(parent)
        , Output(settings)
        , m_client(new QMqttClient(this)){
        m_client->setHostname(settings.mqttHost());
        m_client->setPort(settings.mqttPort());
        m_client->setUsername(settings.mqttUser());
        m_client->setPassword(settings.mqttPassword());
        m_client->setClientId("deye-inverter-addon-" + settings.instance().replace(" ", "-").toLower());
        m_client->setProtocolVersion(QMqttClient::MQTT_3_1_1);
    }

    ~MqttClient() override = default;

    bool init() override{
        if(m_client == nullptr){
            qDebug() << "Failed to create mqtt client";
            return false;
        }

        QObject::connect(m_client, &QMqttClient::connected, []() {
            qDebug() << "MQTT Connected!";
            //Mqtt_publishAutoDiscovery(client, sensors);
        });

        QObject::connect(m_client, &QMqttClient::errorChanged, [](QMqttClient::ClientError error) {
            qCritical() << "MQTT Error:" << error;
        });

        m_client->connectToHost();
        
        return true;
    }

    void deinit() override{
        if(m_client){
            m_client->disconnectFromHost();
            m_client->deleteLater();
            m_client = nullptr;
        }
    }

    void update(const QJsonObject& report) override {
		if (m_client == nullptr) {
			qWarning() << "MQTT client not initialized";
			return;
		}

        if (m_client->state() == QMqttClient::Connected) {
            qDebug() << "MQTT client is connected";
        }
        else {
            qDebug() << "MQTT client is not connected";
            qDebug() << "Current state:" << m_client->state();
            return;
        }


        QObject* sender = QObject::sender();
        Inverter* inv = qobject_cast<Inverter*>(sender);
        if (inv == nullptr) {
            qWarning() << "Sender is not Inverter";
            return;
        }
        
        const auto& sensors = inv->sensors();

        if (m_published == false){
            publishAutoDiscovery(sensors);
            m_published = true;
        }

        for (const auto& sensor : sensors) {
            const QString topic = QString("deye/%1/sensor/%2/state").arg(settings().instance()).arg(sensor.topicSuffix());
            const float value = report[sensor.uniqueId()].toDouble();
            m_client->publish(
                QMqttTopicName(topic),
                QByteArray::number(value, 'f', 2),  // 2 decimal places
                1,  // QoS 1
                true
            );
        }
    }

private:
    void publishAutoDiscovery(const QVector<Sensor>& sensors) {
    
    for (const auto &sensor : sensors) {
        QJsonObject payload {
            {"name", QString("Deye %1 %2").arg(settings().instance()).arg(sensor.name()).toLower()},
            {"state_topic", QString("deye/%1/sensor/%2/state").arg(settings().instance()).arg(sensor.topicSuffix())},
            {"unit_of_measurement", sensor.unit()},
            {"device_class", sensor.deviceClass()},
            {"unique_id", QString("deye_%1_%2").arg(settings().instance()).arg(sensor.uniqueId()).replace(" ","_").toLower()},
            {"availability_topic", QString("deye_%1/status").arg(settings().instance()).replace(" ","_").toLower()}
        };

        m_client->publish(
            QMqttTopicName("homeassistant/sensor/" + QString("%1_%2").arg(settings().instance()).arg(sensor.uniqueId()).replace(" ","_") + "/config"),
            QJsonDocument(payload).toJson(),
            1,  // QoS 1
            true // Retain
        );
    }
    
    // Publish online status
    m_client->publish(QMqttTopicName(QString("deye_%1/status").arg(settings().instance()).replace(" ", "_").toLower()), "online", 1, true);
}

private:
    QMqttClient* m_client = nullptr;
    Settings m_settings;
    bool m_published = false;
};

#endif // MQTTCLIENT_H