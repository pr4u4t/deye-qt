#ifndef UTILS_H
#define UTILS_H

#include <QModbusDevice>
#include <QSerialPort>
#include <QDebug>

auto ModBus_state_string(int state) -> QString;

auto SerialPort_parity_from_string(const QString& parity) ->  QSerialPort::Parity;

#endif