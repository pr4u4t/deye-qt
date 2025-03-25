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
    QString name;
    QString unit;
    QString deviceClass;
    QString topicSuffix;
    QString uniqueId;
    float scalingFactor;
    int address;

    // Add this method
    QString toString() const {
        return QString("DeyeSensor { "
                      "name: '%1', "
                      "unit: '%2', "
                      "deviceClass: '%3', "
                      "topicSuffix: '%4', "
                      "uniqueId: '%5', "
                      "scalingFactor: %6, " 
                      "address: %7 }")
            .arg(name)
            .arg(unit)
            .arg(deviceClass)
            .arg(topicSuffix)
            .arg(uniqueId)
            .arg(scalingFactor)
            .arg(address);
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

    void read(int startAddress, int numRegisters);

    void readReport();

    const QVector<DeyeSensor>& sensors() const;
    
signals:
    void reportReady();

protected slots:
    void onReadReady(QModbusReply* reply, const ValueModifier& mod);

    void onReadReady(QModbusReply* reply);

protected:
    QModbusDataUnit readRequest(int startAddress, int numRegisters);

    std::optional<const DeyeSensor&> find(int address) const; 

private:
    QModbusRtuSerialClient* m_modbusDevice = nullptr;
    QJsonObject* m_model = nullptr;
    QMqttClient* m_client = nullptr;
    QVector<DeyeSensor> m_dict;
};

#endif
