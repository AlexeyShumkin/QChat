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

bool PostHandler::specHandle(QString& str)
{
    QString substr = str.mid(0, str.indexOf(' '));
    query = "SELECT id FROM users WHERE login = '" + substr + "'";
    substr = DBHandler::getConnection()->getData(query);
    substr = substr.mid(0, substr.indexOf('\n'));
    if(str.indexOf('@') == -1)
    {
        query = "INSERT INTO msgdata(sender_id, content) VALUES('" + substr + "','";
        substr = str.mid(str.indexOf(' ') + 1);
        query += substr + "')";
        return DBHandler::getConnection()->executeQuery(query);
    }
    else
    {
        QString sender = substr;
        substr = str.mid(str.indexOf('@') + 1, str.lastIndexOf('@') - str.indexOf('@') - 1);
        query = "SELECT id FROM users WHERE login = '" + substr + "'";
        substr = DBHandler::getConnection()->getData(query);
        substr = substr.mid(0, substr.indexOf('\n'));
        query = "INSERT INTO msgdata(sender_id, recipient_id, content) VALUES('" + sender + "','" + substr + "','";
        substr = str.mid(str.lastIndexOf('@') + 1);
        query += substr + "')";
        return DBHandler::getConnection()->executeQuery(query);
    }

}

bool UsersDisplayHandler::specHandle(QString& str)
{
    query = "SELECT login FROM users";
    str = DBHandler::getConnection()->getData(query);
    return str.size();
}
