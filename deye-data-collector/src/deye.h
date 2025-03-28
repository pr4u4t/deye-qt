#ifndef DEYE_H
#define DEYE_H

#include <QCommandLineParser>
#include <QModbusRtuSerialClient>
#include <QSerialPortInfo>
#include <QVariant>
#include <QtLogging>
#include <QJsonObject>
#include <QMqttClient>
#include <QVector>
#include <QStack>

#include "utils.h"
#include "settings.h"

enum class SensorDataType{
    SHORT = 0,
    USHORT,
    SSHORT,
    DWORD
};

struct dshort {
    qint16 low;
    qint16 high;
};

union SensorData {
    float result;
    qint32 data;
    dshort words;
};

struct DeyeSensor {
    QString name;
    QString unit;
    QString deviceClass;
    QString topicSuffix;
    QString uniqueId;
    float scalingFactor;
    int address;
    SensorDataType type;
    float data;

    QString typeString(SensorDataType type) const{
        switch(type){
            case SensorDataType::SSHORT:
                return "signed short";
            case SensorDataType::SHORT:
                return "short";
            case SensorDataType::USHORT:
                return "unsigned short";
            case SensorDataType::DWORD:
                return "dword";
        }

        return "unknown type";
    }

    // Add this method
    QString toString() const {
        return QString("DeyeSensor { "
                      "name: '%1', "
                      "unit: '%2', "
                      "deviceClass: '%3', "
                      "topicSuffix: '%4', "
                      "uniqueId: '%5', "
                      "scalingFactor: %6, " 
                      "address: %7, " 
                      "type: %8 }")
            .arg(name)
            .arg(unit)
            .arg(deviceClass)
            .arg(topicSuffix)
            .arg(uniqueId)
            .arg(scalingFactor)
            .arg(address)
            .arg(typeString(type));
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
            {"scaling_factor", scalingFactor},
            {"address", address},
            {"type", typeString(type)}
        };
    }
};

class Deye : public QObject {
    Q_OBJECT

public:
    Deye(const Settings& settings, QJsonObject* model = nullptr, QMqttClient *client = nullptr, QObject* parent = nullptr);

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

    void read(int startAddress, int numRegisters, int serverAddress = 1);

    void readReport();

    const QVector<DeyeSensor>& sensors() const;
    
    inline void setMqtt(QMqttClient* client){
        m_client = client;
    }

    inline void setModel(QJsonObject* object){
        m_model = object;
    }

    inline const QMqttClient* mqttClient() const {
        return m_client;
    }

    inline const QJsonObject* model() const {
        return m_model;
    }

signals:
    void reportReady();

protected slots:

    void onReadReady(QModbusReply* reply);

protected:
    QModbusDataUnit readRequest(int startAddress, int numRegisters);

    int find(int address) const; 

    void updateSensor(DeyeSensor &sensor, const QModbusDataUnit& unit);

private:
    float sensorValue(const /*signed*/ qint16 data, float scale) const; //<<<--- signed short
    float sensorValue(const /*unsigned*/ quint16 data, float scale) const; //<<<--- unsigned short
    float sensorValue(const /*signed*/ qint16 low, const qint16 high, float scale) const;

    QVector<DeyeSensor> createSensorList() const;

    QModbusRtuSerialClient* m_modbusDevice = nullptr;
    QJsonObject* m_model = nullptr;
    QMqttClient* m_client = nullptr;
    QVector<DeyeSensor> m_dict;
    QStack<int> m_ops;
};

#endif
