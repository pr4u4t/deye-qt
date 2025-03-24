#ifndef DEYE_H
#define DEYE_H

#include <QModbusRtuSerialClient>
#include <QSerialPortInfo>
#include <QVariant>
#include <QtLogging>
#include <QJsonObject>
#include <QMqttClient>
#include <QVector>

#include "utils.h"
#include "settings.h"

struct DeyeSensor {
    QString name;          // "Battery SOC"
    QString unit;          // "%"
    QString deviceClass;   // "battery"
    QString topicSuffix;   // "battery_soc"
    QString uniqueId;      // "deye_battery_soc_001"
    float scalingFactor;   // 1.0, 0.1, or 0.01 for /10 or /100 values

    // Add this method
    QString toString() const {
        return QString("DeyeSensor { "
                      "name: '%1', "
                      "unit: '%2', "
                      "deviceClass: '%3', "
                      "topicSuffix: '%4', "
                      "uniqueId: '%5', "
                      "scalingFactor: %6 }")
            .arg(name)
            .arg(unit)
            .arg(deviceClass)
            .arg(topicSuffix)
            .arg(uniqueId)
            .arg(scalingFactor);
    }

    friend QDebug operator<<(QDebug debug, const DeyeSensor &sensor) {
        QDebugStateSaver saver(debug);
        debug.nospace() << sensor.toString();
        return debug;
    }

    QJsonObject toJson() const {
        return {
            {"name", name},
            {"unit", unit},
            {"device_class", deviceClass},
            {"topic_suffix", topicSuffix},
            {"unique_id", uniqueId},
            {"scaling_factor", scalingFactor}
        };
    }
};

class Deye : public QObject {
    Q_OBJECT

public:
    Deye(const Settings& settings, QJsonObject* model = nullptr, QMqttClient *client = nullptr, const QVector<DeyeSensor>& dict = QVector<DeyeSensor>(), QObject* parent = nullptr);

    ~Deye(){
        if(m_modbusDevice != nullptr){
            delete m_modbusDevice;
        }
    }

    inline bool connectDevice(){
        if (!m_modbusDevice->connectDevice()) {
            qDebug() << QString("Connect failed: %1").arg(m_modbusDevice->errorString());
            return false;
        }

        return true;
    }

    void read(int startAddress, int numRegisters, const ValueModifier& mod = ValueModifier(), int serverAddr = 1);

    void readReport();

signals:
    void reportReady();

protected slots:
    void onReadReady(QModbusReply* reply, const ValueModifier& mod);

protected:
    QModbusDataUnit readRequest(int startAddress, int numRegisters);

private:
    QModbusRtuSerialClient* m_modbusDevice = nullptr;
    QJsonObject* m_model = nullptr;
    QMqttClient* m_client = nullptr;
    QVector<DeyeSensor> m_dict;
};

#endif
