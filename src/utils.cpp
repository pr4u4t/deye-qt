#include "utils.h"

auto ModBus_state_string(int state) -> QString {
    switch(state){
        case QModbusDevice::UnconnectedState:
            return "The device is disconnected.";
        case QModbusDevice::ConnectingState:
            return "The device is being connected.";
        case QModbusDevice::ConnectedState:
            return "The device is connected to the Modbus network.";
        case QModbusDevice::ClosingState:
            return "The device is being closed.";
    }

    return "Unknown state";
}

auto SerialPort_parity_from_string(const QString& parity) ->  QSerialPort::Parity{
    if(parity == "none"){
        return QSerialPort::NoParity;
    }
    
    if(parity == "even"){
        return QSerialPort::EvenParity;
    } 
    
    if(parity == "odd"){
        return QSerialPort::OddParity;
    }
    
    if(parity == "mark"){
        return QSerialPort::MarkParity;
    }
    
    if(parity == "space"){
        return QSerialPort::SpaceParity;
    }
    
    qDebug() << "No valid parity defaulting to none";

    return QSerialPort::NoParity;
}