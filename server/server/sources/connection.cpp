#include "connection.h"
#include <QMessageBox>
#include <QtSql>

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




