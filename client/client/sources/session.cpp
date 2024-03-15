#include "session.h"
#include <QDataStream>

Session::Session()
{
    socket = new QTcpSocket(this);
    socket->connectToHost("127.0.0.1", 7066);
    connect(socket, &QTcpSocket::readyRead, this, &Session::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void Session::sendToServer(QString str)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << str;
    socket->write(data);
}

QString Session::getBuffer() const
{
    return buffer;
}

void Session::slotReadyRead()
{
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_5_15);
    if(in.status() == QDataStream::Ok)
    {
        in >> buffer;
    }
}

