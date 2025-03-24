#ifndef SETTINGS_H
#define SETTINGS_H

#include <QCommandLineParser>
#include <QJsonObject>


//typedef struct _Settings Settings;
struct Settings{
    Settings() = default;

    Settings(const QCommandLineParser& parser){
        fillFromCmd(parser);
    }

    Settings(const QJsonObject& object){
        fillFromJson(object);
    }

    void fillFromJson(const QJsonObject& object){
        fill<QJsonObject>(object, [](const QJsonObject& source, const QString& key){
            return source.contains(key) == true;
        });
    }

    void fillFromCmd(const QCommandLineParser& parser){
        fill<QCommandLineParser>(parser, [](const QCommandLineParser& source, const QString& key){
            return source.isSet(key) == true;
        });
    }

    template<typename T>
    QString convert(const T& value) const{
	return value.toString();
    }

    QString convert(const QString& value)const{
    	return value;
    }

    template<typename T>
    void fill(const T& source, const std::function<bool (const T&, const QString&)>& hasValue){
        if(hasValue(source, "device") == true){
            device = convert(source.value("device"));
        }
        if(hasValue(source, "parity") == true){
            parity = convert(source.value("parity"));
        }
        if(hasValue(source, "baud") == true){
            baud = source.value("baud").toInt();
        }
        if(hasValue(source, "dataBits") == true){
            dataBits = source.value("dataBits").toInt();
        }
        if(hasValue(source, "stopBits") == true){
            stopBits = source.value("stopBits").toInt();
        }
        if(hasValue(source, "responseTime") == true){
            responseTime = source.value("responseTime").toInt();
        }
        if(hasValue(source, "numberOfRetries") == true){ 
            numberOfRetries = source.value("numberOfRetries").toInt();
        }
        if(hasValue(source, "listen") == true){
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
