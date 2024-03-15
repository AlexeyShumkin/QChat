#pragma once
#include <QString>
#include <QTcpSocket>


class Session : public QTcpSocket
{
    Q_OBJECT

public:
    Session();
    void sendToServer(QString str);
    QString getBuffer() const;
public slots:
    void slotReadyRead();
private:
    QTcpSocket* socket;
    QByteArray data;
    QString buffer;
    quint16 nextBlockSize{0};
};
