#include "session.h"
#include <QDataStream>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

Session::Session()
{
    QFile file("config.json");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open file";
        return;
    }
    QByteArray data = file.readAll();
    file.close();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
    QJsonObject jsonObj = jsonDoc.object();
    socket = new QTcpSocket(this);
    socket->connectToHost(jsonObj["host"].toString(), jsonObj["port"].toInt());
    connect(socket, &QTcpSocket::readyRead, this, &Session::slotReadyRead);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

Session::~Session()
{
   socket->close();
}

void Session::sendToServer(QString str)
{
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << str;
    out.device()->seek(0);
    out << quint16(data.size() - sizeof(quint16));
    socket->write(data);
}

QString& Session::getBuffer()
{
    return buffer;
}

void Session::slotReadyRead()
{
    QDataStream in(socket);

    in.setVersion(QDataStream::Qt_5_15);
    if(in.status() == QDataStream::Ok)
    {
        for(;;)
        {
            if(nextBlockSize == 0)
            {
                if(socket->bytesAvailable() < 2) break;
                in >> nextBlockSize;
            }
            if(socket->bytesAvailable() < nextBlockSize) break;

            in >> buffer;
            nextBlockSize = 0;
        }
    }
}

bool Session::check()
{
    return socket->bytesAvailable() == 0;
}

bool Session::socketBufferBlocked()
{
    if(!socket->signalsBlocked())
    {
        socket->setSocketOption(QAbstractSocket::LowDelayOption, 0);
    }
    else
    {
        socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    }
    return socket->signalsBlocked();
}

void Session::clearBuffer()
{
    buffer.clear();
}

