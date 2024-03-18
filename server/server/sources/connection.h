#pragma once
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>

class DBHandler
{
public:
    ~DBHandler();
    DBHandler(const DBHandler& other) = delete;
    DBHandler& operator=(const DBHandler& other) = delete;

    bool executeQuery(QString& str);
    QString& getData(QString& str);

    static DBHandler* getConnection();
    bool isConnected() const;

private:
    DBHandler();
    QSqlDatabase db;
};


