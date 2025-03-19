#ifndef DEYE_H
#define DEYE_H

#include <QModbusRtuSerialClient>
#include <QSerialPortInfo>
#include <QVariant>
#include <QtLogging>

#include "utils.h"

class Deye : public QObject {
    Q_OBJECT

public:
    Deye(const ModBusSettings& settings, QObject* parent = nullptr);

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
};

#endif