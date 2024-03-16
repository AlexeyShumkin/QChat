#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <memory>
#include "handler.h"

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    void serverUp(QString& str);
    QTcpSocket* socket_;
    bool check(QString& str);

private:
    QVector<QTcpSocket*> sockets_;
    QByteArray data_;
    quint16 nextBlockSize{0};
    enum Commands { SIGNUP = 1, SIGNIN, PUB };
    std::unique_ptr<IHandler> handler;

    void sendToClient(QString str);
    void setHandler(std::unique_ptr<IHandler>&& h);

public slots:
    void incomingConnection(qintptr socketDecriptor);
    void slotReadyRead();
};
