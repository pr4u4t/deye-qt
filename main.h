#ifndef MAIN_H
#define MAIN_H

#include <QCoreApplication>
#include <QModbusRtuSerialClient>
#include <QSerialPortInfo>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QLoggingCategory>
#include <QSerialPort>

struct ValueModifier{
    float scale = 1.0f;
    QString unit;
    QString name;
};

struct ModBusSettings {
    ModBusSettings() = default;

    ModBusSettings(
        const QString& port, const int parity, const int baud, 
        const int dataBits, const int stopBits, const int responseTime, 
        const int numberOfRetries
    )
    : m_port(port)
    , m_parity(parity)
    , m_baud(baud)
    , m_dataBits(dataBits)
    , m_stopBits(stopBits)
    , m_responseTime(responseTime)
    , m_numberOfRetries(numberOfRetries){}

    ModBusSettings(const QString& port)
    : m_port(port){}

    QString toString() const {
        return QString("Port: %1, Parity: %2, Baud: %3, DataBits: %4, StopBits: %5, ResponseTime: %6ms, Retries: %7")
            .arg(m_port)
            .arg(m_parity)
            .arg(m_baud)
            .arg(m_dataBits)
            .arg(m_stopBits)
            .arg(m_responseTime)
            .arg(m_numberOfRetries);
    }

    QString m_port;
    int m_parity = QSerialPort::EvenParity;
    int m_baud = QSerialPort::Baud19200;
    int m_dataBits = QSerialPort::Data8;
    int m_stopBits = QSerialPort::OneStop;
    int m_responseTime = 1000;
    int m_numberOfRetries = 3;
};

#endif