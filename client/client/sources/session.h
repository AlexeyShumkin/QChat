#pragma once
#include <QString>
#include <QTcpSocket>
#include <mutex>

class Session : public QTcpSocket
{
    Q_OBJECT

public:
    Session();
    ~Session();
    void sendToServer(QString str);
    QString& getBuffer();
    bool check();

public slots:
    void slotReadyRead();
private:
    QTcpSocket* socket;
    QByteArray data;
    QString buffer;
    quint16 nextBlockSize{0};
};
