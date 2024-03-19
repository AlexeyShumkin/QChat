#pragma once

#include <QTcpServer>
#include <QTcpSocket>
#include <memory>
#include "handler.h"
#include <QTimer>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    void serverUp(QString& str);
    QTcpSocket* socket;

private:
    QMap<QTcpSocket*, int> sockets;
    QByteArray data;
    quint16 nextBlockSize{ 0 };
    enum Commands { SIGNUP = 1, SIGNIN, POST, USERS, SIGNOUT, UPDATE };
    std::unique_ptr<IHandler> handler;
    void sendToClient(const QString& str);
    void setHandler(std::unique_ptr<IHandler>&& h);
signals:
    void clientResponse();
public slots:
    void incomingConnection(qintptr socketDecriptor);
    void slotReadyRead();
    void checkForBlock(int id);
    void checkForUnBlock(int id);
    void unblock();
};
