#include "connection.h"
#include <QMessageBox>
#include <QtSql>
#include <QDebug>

DBHandler::~DBHandler()
{
    if(db.isOpen())
    {
        db.close();
    }
}

DBHandler::DBHandler()
{
    db = QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName("servdb");
    db.setUserName("chat");
    db.setPassword("12345");
    db.setHostName("localhost");
    db.open();
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
    str.clear();
    for(size_t i = 0; query.next(); ++i)
    {
        str += query.value(i).toString();
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




