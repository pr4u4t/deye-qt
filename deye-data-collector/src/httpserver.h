#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QHttpServer>
#include <QTcpServer>
#include <QJsonObject>
#include <QJsonDocument>

#include "output.h"
#include "settings.h"

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

    bool init();

    void deinit();

private:
    QHttpServer* m_server = nullptr;
    QJsonObject m_data;
    QTcpServer* m_tcpserver = nullptr;
};

#endif // HTTPSERVER_H