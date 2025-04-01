#include "httpserver.h"

bool HttpServer::init(){
    if(m_server == nullptr){
        qDebug() << "Failed to create http server";
        return false;
    }

    if(m_tcpserver == nullptr){
        qDebug() << "Failed to create tcp server";
        return false;
    }
    
    if (!m_tcpserver->listen(QHostAddress::Any, settings().listen()) || !m_server->bind(m_tcpserver)) {
        qCritical() << "Failed to start server!";
        return false;
    }

    m_server->route("/sensor", QHttpServerRequest::Method::Get, [this] (const QHttpServerRequest &request, QHttpServerResponder &responder) {
        Q_UNUSED(request);
        responder.write(QJsonDocument(m_data).toJson(), "application/json");
    });

    return true;
}

void HttpServer::deinit(){
    if(m_server){
        delete m_server;
        m_server = nullptr;
    }

    if(m_tcpserver){
        m_tcpserver->close();
        delete m_tcpserver;
        m_tcpserver = nullptr;
    }
}