#ifndef DEYE_H
#define DEYE_H

#include <QModbusRtuSerialClient>
#include <QSerialPortInfo>
#include <QVariant>
#include <QtLogging>
#include <QJsonObject>

#include "utils.h"
#include "settings.h"

class Deye : public QObject {
    Q_OBJECT

public:
    Deye(const Settings& settings, QJsonObject* model = nullptr, QObject* parent = nullptr);

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
    QJsonObject* m_model;
};

struct DeyeSensor {
    QString name;          // "Battery SOC"
    QString unit;          // "%"
    QString deviceClass;   // "battery"
    QString topicSuffix;   // "battery_soc"
    QString uniqueId;      // "deye_battery_soc_001"
    float scalingFactor;   // 1.0, 0.1, or 0.01 for /10 or /100 values
};

#endif
