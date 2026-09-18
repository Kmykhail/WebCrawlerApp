#ifndef MOCKHTTPSERVER_H
#define MOCKHTTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QByteArray>

class MockHttpServer : public QObject
{
    Q_OBJECT
public:
    explicit MockHttpServer(QObject *parent = nullptr);

    bool listen();
    quint16 port() const;

    void addRoute(const QString &path, quint16 statusCode, const QByteArray &body, quint32 delay = 0);

private:
    void handleConnection(QTcpSocket *socket);

    struct Response {
        quint16 statusCode;
        QByteArray body;
        quint32 delay;
    };


private:
    QTcpServer *m_server;
    QMap<QString, Response> m_routes;

};

#endif // MOCKHTTPSERVER_H
