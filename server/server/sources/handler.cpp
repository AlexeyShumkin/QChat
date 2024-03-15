#include "handler.h"
#include "connection.h"

bool SignUpHandler::specHandle(QString& str)
{
    QString substr = str.mid(0, str.indexOf(' '));
    query = "INSERT INTO users(login, pw_hash, status) VALUES('" + substr + "','";
    substr = str.mid(str.indexOf(' ') + 1);
    query += substr + "','online')";
    return DBHandler::getConnection()->executeQuery(query);
}

bool SignInHandler::specHandle(QString& str)
{
    QString substr = str.mid(0, str.indexOf(' '));
    query = "SELECT id FROM users WHERE login = '" + substr + "' AND pw_hash = '";
    substr = str.mid(str.indexOf(' ') + 1);
    query += substr + "'";
    auto result = DBHandler::getConnection()->getData(query);
    return result.size();
}
