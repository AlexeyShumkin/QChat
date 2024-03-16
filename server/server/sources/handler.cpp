#include "handler.h"
#include "connection.h"
#include <QDebug>

bool SignUpHandler::specHandle(QString& str)
{
    QString substr1 = str.mid(0, str.indexOf(' '));
    QString substr2 = str.mid(str.indexOf(' ') + 1);
    query = "INSERT INTO users(login, pw_hash, status) VALUES('" + substr1 + "','" + substr2 + "','online')";
    bool check = DBHandler::getConnection()->executeQuery(query);
    if(!check) return false;
    query = "SELECT id FROM users WHERE login = '" + substr1 + "'";
    str = DBHandler::getConnection()->getData(query);
    return str.size();
}

bool SignInHandler::specHandle(QString& str)
{
    QString substr = str.mid(0, str.indexOf(' '));
    query = "SELECT id FROM users WHERE login = '" + substr + "' AND pw_hash = '";
    substr = str.mid(str.indexOf(' ') + 1);
    query += substr + "'";
    str = DBHandler::getConnection()->getData(query);
    return str.size();
}
