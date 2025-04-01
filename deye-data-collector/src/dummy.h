#ifndef DUMMY_H
#define DUMMY_H

#include <QCommandLineParser>
#include <QModbusRtuSerialClient>
#include <QSerialPortInfo>
#include <QVariant>
#include <QtLogging>
#include <QJsonObject>
#include <QVector>
#include <QStack>

#include <random>
#include <limits>

#include "utils.h"
#include "settings.h"
#include "inverter.h"

class Dummy : public Inverter {

    Q_OBJECT

public:
    Dummy(const Settings& settings, QObject* parent = nullptr);

    ~Dummy();

    bool connectDevice();

    void disconnectDevice();

    void readReport();

    const QVector<Sensor>& sensors() const;

private:
    float sensorValue(const /*signed*/ qint16 data, float scale) const; //<<<--- signed short
    float sensorValue(const /*unsigned*/ quint16 data, float scale) const; //<<<--- unsigned short
    float sensorValue(const /*signed*/ qint16 low, const qint16 high, float scale) const;

    template<typename T>
    T generateRandom(T min = std::numeric_limits<T>::min(),T max = std::numeric_limits<T>::max()) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<T> dist(min, max);
        return dist(gen);
    }


    QVector<Sensor> createSensorList() const;
    QVector<Sensor> m_dict;
};

#endif