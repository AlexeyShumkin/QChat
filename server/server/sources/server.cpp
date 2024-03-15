#include "server.h"
#include <QDataStream>

Server::Server()
{
    if(this->listen(QHostAddress::Any, 7066))
    {
        qDebug() << "start";
    }
    else
    {
        qDebug() << "error";
    }
}

void Server::serverUp(QString& str)
{
    QString p = str.mid(0, 1);
    switch(p.toInt())
    {
    case SIGNUP:
        setHandler(std::make_unique<SignUpHandler>());
        break;
    case SIGNIN:
        setHandler(std::make_unique<SignInHandler>());
        break;
    }
    str = str.mid(str.indexOf('#') + 1);
}

bool Server::check(QString &str)
{
    serverUp(str);
    return handler->specHandle(str);
}

void Server::sendToClient(QString str)
{
    data_.clear();
    QDataStream out(&data_, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << str;
    for(auto socket : sockets_)
    {
        socket->write(data_);
    }
}

void Server::setHandler(std::unique_ptr<IHandler>&& h)
{
    handler = std::move(h);
}

void Server::incomingConnection(qintptr socketDecriptor)
{
    socket_ = new QTcpSocket;
    socket_->setSocketDescriptor(socketDecriptor);
    connect(socket_, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket_, &QTcpSocket::disconnected, socket_, &QTcpSocket::deleteLater);
    sockets_.push_back(socket_);
    qDebug() << "client connected" << socketDecriptor;
}

void Server::slotReadyRead()
{
    socket_ = (QTcpSocket*)sender();
    QDataStream in(socket_);
    in.setVersion(QDataStream::Qt_5_15);
    if(in.status() == QDataStream::Ok)
    {
        qDebug() << "read...";
        QString str;
        in >> str;
        serverUp(str);
        if(handler->specHandle(str))
        {
            qDebug() << str;
            str.clear();
            str.push_back('1');
            sendToClient(str);
        }
        else
        {
            str.clear();
            str.push_back('2');
            sendToClient(str);
        }
    }
    else
    {
        qDebug() << "Datastream error";
    }
}

