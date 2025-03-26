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

        if(hasValue(source, "instance") == true){
            instance = convert(source.value("instance"));
        }

        if(hasValue(source, "baud") == true){
            baud = source.value("baud").toInt();
        }

        if(hasValue(source, "data-bits") == true){
            dataBits = source.value("data-bits").toInt();
        }

        if(hasValue(source, "stop-bits") == true){
            stopBits = source.value("stop-bits").toInt();
        }

        if(hasValue(source, "response-time") == true){
            responseTime = source.value("response-time").toInt();
        }

        if(hasValue(source, "number-of-retries") == true){ 
            numberOfRetries = source.value("number-of-retries").toInt();
        }

        if(hasValue(source, "listen") == true){
            listen = source.value("listen").toInt();
        }

        if(hasValue(source, "interval") == true){
            listen = source.value("interval").toInt();
        }

        if(hasValue(source, "http-server") == true){
            qDebug() << source.value("http-server");
            httpserver = source.value("http-server").toInt();
        }
    }

    QString toString() const {
        return QString(
            "Settings("
            "device: %1, parity: %2, instance: %3, "
            "baud: %4, data-bits: %5, stop-bits: %6, "
            "response-time: %7, number-of-retries: %8, "
            "listen: %9, interval: %10, http-server: %11)"
        )
        .arg(device)
        .arg(parity)
        .arg(instance)
        .arg(baud)
        .arg(dataBits)
        .arg(stopBits)
        .arg(responseTime)
        .arg(numberOfRetries)
        .arg(listen)
        .arg(interval)
        .arg(httpserver);
    }

    // QDebug output support
    friend QDebug operator<<(QDebug debug, const Settings &settings) {
        debug.nospace().noquote() << settings.toString();
        return debug;
    }

    QString device = "/dev/ttyUSB0";
    QString parity = "none";
    QString instance = "Instance 1";
    int baud = 9600;
    int dataBits = 8;
    int stopBits = 1;
    int responseTime = 1000; 
    int numberOfRetries = 3;
    int listen = 8080;
    int interval = 5000;
    bool httpserver = false;
};

#endif
