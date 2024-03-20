#include "connection.h"
#include <QMessageBox>
#include <QtSql>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

DBHandler::~DBHandler()
{
    if(db.isOpen())
    {
        db.close();
    }
}

DBHandler::DBHandler()
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
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName(jsonObj["dbName"].toString());
    db.setUserName(jsonObj["userName"].toString());
    db.setPassword(jsonObj["password"].toString());
    db.setHostName(jsonObj["host"].toString());
    db.open();
}

const QSqlDatabase& DBHandler::getDb() const
{
    return db;
}

bool DBHandler::executeQuery(QString& str)
{
    QSqlQuery query(db);
    return query.exec(str);
}

QString& DBHandler::getData(QString& str)
{
    QSqlQuery query(db);
    query.exec(str);
    QSqlRecord rec = query.record();
    str.clear();
    while(query.next())
    {
        for(int i = 0; i < rec.count(); ++i)
        {
            if(i != rec.count() - 1)
            {
                str += query.value(i).toString() + ' ';
            }
            else
            {
                str += query.value(i).toString() + '\n';
            }
        }
    }
    return str;
}

DBHandler* DBHandler::getConnection()
{
    static DBHandler connection;
    return connection.isConnected()? &connection : nullptr;
}

bool DBHandler::isConnected() const
{
    return db.isOpen();
}




