#include "mockhttpserver.h"

#include <QDebug>
#include <QTextStream>
#include <QAbstractSocket>
#include <QTimer>

MockHttpServer::MockHttpServer(QObject *parent)
    : QObject{parent}
    , m_server{new QTcpServer(this)}
{
    connect(m_server, &QTcpServer::newConnection, this, [this](){
        qDebug() << "NEW CONNECTION";
        while (m_server->hasPendingConnections()) {
            QTcpSocket *socket = m_server->nextPendingConnection();
            handleConnection(socket);
            connect(socket, &QAbstractSocket::disconnected, socket, &QObject::deleteLater);
        }
    });
}

bool MockHttpServer::listen()
{
    return m_server->listen(QHostAddress::LocalHost, 0);
}

quint16 MockHttpServer::port() const
{
    return m_server->serverPort();
}

void MockHttpServer::addRoute(const QString &path, quint16 statusCode, const QByteArray &body, quint32 delay)
{
    m_routes[path] = {statusCode, body, delay};
}

void MockHttpServer::handleConnection(QTcpSocket *socket)
{
    if (!socket->waitForReadyRead(1000)) {
        socket->disconnectFromHost();
        return;
    }

    QTextStream stream(socket);
    QString requestLine = stream.readLine();
    QStringList parts = requestLine.split(' ');

    QString path = "/";
    if (parts.size() >= 2) {
        path = parts[1];
    }

    quint16 status = 404;
    QByteArray body = "Not Found";
    quint32 delay = 0;

    if (m_routes.contains(path)) {
        status = m_routes[path].statusCode;
        body = m_routes[path].body;
        delay = m_routes[path].delay;
    }

    auto sendResponse = [socket, status, body]() {
        if (!socket || !socket->isOpen()) return;

        QString statusText = (status == 200) ? "OK" : "Not Found";
        QByteArray response =
            QString("HTTP/1.1 %1 %2\r\n"
                    "Content-Length: %3\r\n"
                    "Content-Type: text/html\r\n"
                    "Connection: close\r\n\r\n")
                .arg(status)
                .arg(statusText)
                .arg(body.size())
                .toUtf8() + body;

        socket->write(response);
        socket->flush();
        socket->disconnectFromHost();
    };

    if (delay > 0) {
        QTimer::singleShot(delay, socket, sendResponse);
    } else {
        sendResponse();
    }
}
