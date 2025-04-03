#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <QMqttClient>
#include <QJsonDocument>
#include <QJsonObject>
#include <QHash>

#include "inverter.h"
#include "settings.h"
#include "output.h"

class MqttClient : public Output {
    Q_OBJECT
public:

    MqttClient(const Settings& settings, QObject* parent = nullptr);

    ~MqttClient() override;

    bool init() override;

    void deinit() override;

    void update(const QJsonObject& report) override;

private:
    void publishAutoDiscovery(const QVector<Sensor>& sensors, const QString& instance);

private:
    QMqttClient* m_client = nullptr;
    Settings m_settings;
    QHash<QString, bool> m_init;
};

#endif // MQTTCLIENT_H