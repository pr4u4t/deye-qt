#ifndef DEYE_H
#define DEYE_H

#include <QCommandLineParser>
#include <QModbusRtuSerialClient>
#include <QSerialPortInfo>
#include <QVariant>
#include <QtLogging>
#include <QJsonObject>
#include <QVector>
#include <QStack>

#include "utils.h"
#include "settings.h"
#include "inverter.h"

class Deye  :  public Inverter {
    
    Q_OBJECT

public:
    Deye(const Settings& settings, QObject* parent = nullptr);

    ~Deye();

    bool connectDevice();

    void disconnectDevice();

    void readReport();

    const QVector<Sensor>& sensors() const;

protected slots:

    void onReadReady(QModbusReply* reply, int startAddress);

protected:
    void read(int startAddress, int numRegisters, int serverAddress = 1);

    QModbusDataUnit readRequest(int startAddress, int numRegisters);

    int find(int address) const; 

    void updateSensor(Sensor &sensor, const QModbusDataUnit& unit);

private:
    float sensorValue(const /*signed*/ qint16 data, float scale) const; //<<<--- signed short
    float sensorValue(const /*unsigned*/ quint16 data, float scale) const; //<<<--- unsigned short
    float sensorValue(const /*signed*/ qint16 low, const qint16 high, float scale) const;

    QVector<Sensor> createSensorList() const;

    QModbusRtuSerialClient* m_modbusDevice = nullptr;
    QVector<Sensor> m_dict;
    QStack<int> m_ops;
};

#endif
