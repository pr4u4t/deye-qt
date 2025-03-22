#ifndef MAIN_H
#define MAIN_H

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QLoggingCategory>

#include "deye.h"

//typedef struct _Settings Settings;
struct Settings{
    Settings(const QCommandLineParser& parser){
        fillFromCmd(parser);
    }

    Settings(const QJsonObject& object){
        fillFromJson(object);
    }

    void fillFromJson(const QjsonObject& object){
        fill<QJsonObject>(object, [](const QJsonObject& source, const QString& key){
            return source.find(key).size() > 0;
        });
    }

    void fillFromCmd(const QCommandLineParser& parser){
        fill<QCommandLineParser>(parser,[](){
            return source.isSet(key);
        });
    }

    template<typename T>
    void fill(const T& source, const std::function<bool (const T&, const QString&)>& hasValue){
        if((hasValue(source, "device")) == true){
            device = source.value("device").toString();
        }
        if((hasValue(source, "parity")) == true){
            parity = source.value("parity").toString();
        }
        if(if(hasValue(source, "baud")) == true){
            baud = source.value("baud").toInt();
        }
        if(hasValue(source, "dataBits")) == true){
            dataBits = source.value("dataBits").toInt();
        }
        if(if(hasValue(source, "stopBits")) == true){
            stopBits = source.value("stopBits").toInt();
        }
        if((hasValue(source, "responseTime")) == true){
            responseTime = source.value("responseTime").toInt();
        }
        if((hasValue(source, "numberOfRetries")) == true){ 
            numberOfRetries = source.value("numberOfRetries").toInt();
        }
        if((hasValue(source, "listen")) == true){
            listen = source.value("listen").toInt();
        }
    }

    QString device = "/dev/ttyUSB0";
    QString parity = "none";
    int baud = 9600;
    int dataBits = 8;
    int stopBits = 2;
    int responseTime = 1000; 
    int numberOfRetries = 3;
    int listen = 8080;
};

#endif