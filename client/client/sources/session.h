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
    bool socketBufferBlocked();
    void clearBuffer();
public slots:
    void slotReadyRead();
signals:
    void signalForButtons(QString& str);
private:
    QTcpSocket* socket;
    QByteArray data;
    QString buffer;
    quint16 nextBlockSize{0};
    bool userBlockedFlag{false};
};
