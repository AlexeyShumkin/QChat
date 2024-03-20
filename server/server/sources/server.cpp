#include "server.h"
#include <QDataStream>
#include <QTimer>
#include <QSignalBlocker>
#include <mutex>

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
    case POST:
        setHandler(std::make_unique<PostHandler>());
        break;
    case USERS:
        setHandler(std::make_unique<UsersDisplayHandler>());
        break;
    case SIGNOUT:
        setHandler(std::make_unique<SignOutHandler>());
        break;
    case UPDATE:
        setHandler(std::make_unique<UpdateHandler>());
        break;
    }
    str = str.mid(str.indexOf('#') + 1);
}

void Server::sendToClient(const QString& str)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(data.size() - sizeof(quint16));
    socket->write(data);
    emit clientResponse();
}

void Server::setHandler(std::unique_ptr<IHandler>&& h)
{
    handler = std::move(h);
}

void Server::incomingConnection(qintptr socketDecriptor)
{
    socket = new QTcpSocket;
    socket->setSocketDescriptor(socketDecriptor);
    connect(socket, &QTcpSocket::readyRead, this, &Server::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    sockets.insert(socket, 0);
    qDebug() << "client connected" << socket->peerAddress().toString();
}

void Server::slotReadyRead()
{
    socket = (QTcpSocket*)sender();
    if(sockets.find(socket).key()->signalsBlocked())
    {
        socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
        return;
    }
    else
    {
        socket->setSocketOption(QAbstractSocket::LowDelayOption, 0);
    }
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_15);
    if(in.status() == QDataStream::Ok)
    {
        QString str;
        for(;;)
        {
            if(nextBlockSize == 0)
            {
                if(socket->bytesAvailable() < 2) break;
                in >> nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize) break;
            in >> str;
            nextBlockSize = 0;
            break;
        }
        serverUp(str);
        if(str.toInt() && !sockets[socket])
        {
            sockets[socket] = str.toInt();
        }
        if(handler->specHandle(str))
        {
            sendToClient(str);
        }
        else
        {
            str = "-1";
            sendToClient(str);
        }
    }
    else
    {
        qDebug() << "Datastream error";
    }
}

void Server::checkForUnBlock(int id)
{
    for(auto it = sockets.begin(); it != sockets.end(); ++it)
    {
        if(it.value() == id && it.key()->signalsBlocked())
        {
            std::mutex m;
            {
                std::lock_guard<std::mutex> l(m);
                socket = it.key();
                sendToClient("you are unblocked");
            }
            it.key()->blockSignals(false);
        }
    }
    qDebug() << "user with id" << id << "was unblocked!";
}

void Server::disconnect(int id)
{
    for(auto it = sockets.begin(); it != sockets.end(); ++it)
    {
        if(it.value() == id)
        {
            std::mutex m;
            {
                std::lock_guard<std::mutex> l(m);
                socket = it.key();
                sendToClient("you gonna be disconnected");
            }
            it.key()->disconnectFromHost();
        }
    }
    qDebug() << "user with id" << id << "was disconnected!";
}

void Server::checkForBlock(int id)
{
    for(auto it = sockets.begin(); it != sockets.end(); ++it)
    {
        if(it.value() == id)
        {
            std::mutex m;
            {
                std::lock_guard<std::mutex> l(m);
                socket = it.key();
                sendToClient("you are blocked");
            }
            it.key()->blockSignals(true);
        }
    }
    qDebug() << "user with id" << id << "was blocked!";
}

