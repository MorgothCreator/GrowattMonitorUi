#include <QObject>
#include <QFile>
#include <QThread>

#include "webserver.h"
#include "static.h"

WebServer::WebServer(QObject *parent)
    : QObject{parent} {
    server = new QTcpServer(this);
    //connect(server, SIGNAL(newConnection()),this, SLOT(myConnection()));
    if(!server->listen(QHostAddress::Any,8080))
        qDebug() << "\nWeb server     could not start";
    else
        qDebug() <<"\nWeb server is waiting for a connection on port 8080";

    connect(server, &QTcpServer::newConnection, this, [=] {
        socket = server->nextPendingConnection(); //wait for the next pending connection

        //get the information of the current socket, ip address, port number
        QString ip = socket->peerAddress().toString();
        quint16 portNum = socket->peerPort();

        QString str = QString("%1:%2 Connected successfully!").arg(ip).arg(portNum);
        qDebug() << str;

        connect(socket, &QTcpSocket::readyRead, this, [=] {
            //get the text from the text edit write box
            if(!socket)
                return;
            QString str = socket->readAll();

            QStringList list = str.split("\n");
            QString path;
            for (int i = 0; i < list.count(); i++) {
                if(list[i].contains("Referer:")) {
                    QStringList split = list[i].split(": ")[1].remove("\r").split("8080/");
                    if(split.count() > 1)
                        path = split[1];
                }
            }
            socket->write("HTTP/1.1 200 OK\r\n");       // \r needs to be before \n
            socket->write("Content-Type: application/json; charset=utf-8\r\n");
            socket->write("Connection: close\r\n");
            socket->write("Refresh: 1\r\n");     //refreshes web browser every second. Require two \r\n.
            socket->write("Pragma: no-cache\r\n");
            socket->write("\r\n");
            //socket->write("<!DOCTYPE JSON>\r\n");
            //socket->write("<html><body>");
            QFile file;
            file.setFileName(Static::getSamplePath() + path);
            if( file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                socket->write(file.readAll());
                file.close();
            } else {
                socket->write("Invalid path");
            }
            socket->disconnectFromHost();
            socket->close();
            //socket->deleteLater();
            socket = nullptr;
        });
    });
}

WebServer::~WebServer() {
    qDebug() << "\nclosing socket\n";
    if(socket)
        socket->close();
}
