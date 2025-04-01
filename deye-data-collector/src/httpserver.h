#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QHttpServer>
#include <QTcpServer>
#include <QJsonObject>
#include <QJsonDocument>

#include "output.h"
#include "settings.h"
#include "inverter.h"

class HttpServer : public QObject
                 , public Output {
    Q_OBJECT

public:
    HttpServer(const Settings& settings, QObject* parent = nullptr)
        : QObject(parent)
        , Output(settings)
        , m_server(new QHttpServer(this))
        , m_tcpserver(new QTcpServer(this)){
    }

    ~HttpServer() override = default;

    bool init() override;

    void deinit() override;

public slots:
    void update(const QJsonObject& report) override {
		QObject* sender = QObject::sender();
        if (sender == nullptr) {
            qCritical() << "Sender == nullptr";
            return;
        }

        Inverter* inv = qobject_cast<Inverter*>(sender);
		if (inv == nullptr) {
			qCritical() << "Invalid sender";
			return;
		}

		const auto& sensors = inv->sensors();

        for (const auto& sensor : sensors) {
            m_data[QString("deye_%1_sensor_%2_state").arg(settings().instance()).arg(sensor.topicSuffix()).replace(" ", "_").toLower()] = qRound(report[sensor.uniqueId()].toDouble() * 100.0) / 100.0; ;
        }        
    }

private:
    QHttpServer* m_server = nullptr;
    QJsonObject m_data;
    QTcpServer* m_tcpserver = nullptr;
};

#endif // HTTPSERVER_H