#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

class WebServer : public QObject
{
    Q_OBJECT
public:
    explicit WebServer(QObject *parent = nullptr);
    ~WebServer();
    QTcpSocket *socket = nullptr;
private:
    qint64 bytesAvailable() const;
    QTcpServer *server;
};

#endif // WEBSERVER_H
