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
    Dummy(const InverterSettings& settings, QObject* parent = nullptr);

    virtual ~Dummy();

    bool connectDevice() override;

    void disconnectDevice() override;

    void readReport() override;

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

    int getNumberFromEnd(const QString& str) {
        // Create a regular expression to match one or more digits at the end of the string
        QRegularExpression regex("(\\d+)$");

        // Search for the pattern in the input string
        QRegularExpressionMatch match = regex.match(str);

        // If a match is found, convert it to an integer and return
        if (match.hasMatch()) {
            return match.captured(1).toInt();
        }

        // Return -1 or another value to indicate no number was found
        return -1;
    }

    QVector<Sensor> createSensorList() const;
};

#endif