#include "handler.h"
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
    QString login = str.mid(0, str.indexOf(' '));
    QString hash = str.mid(str.indexOf(' ') + 1);
    query = "SELECT id FROM users WHERE login = '" + login + "' AND pw_hash = '" + hash + "'";
    str = DBHandler::getConnection()->getData(query);
    if(!str.size()) return false;
    query = "UPDATE users SET status = 'online' WHERE login = '" + login + "'";
    bool check = DBHandler::getConnection()->executeQuery(query);
    return check && str.size();
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
    query = "SELECT login, status FROM users";;
    str = DBHandler::getConnection()->getData(query);
    return str.size();
}

bool SignOutHandler::specHandle(QString& str)
{
    query = "UPDATE users SET status = 'offline' WHERE id = " + str;
    return DBHandler::getConnection()->executeQuery(query);
}

bool UpdateHandler::specHandle(QString& str)
{
    query = "(SELECT u.login AS sender_login, 'all' AS recipient_login, m.content, to_char(m.received_at,'YYYY-MM-DD HH24:MI:SS') "
            "AS sent, 'public' AS message_type FROM msgdata m JOIN users u ON m.sender_id = u.id WHERE m.recipient_id IS NULL) "
            "UNION ALL "
            "(SELECT u.login AS sender_login, r.login AS recipient_login, m.content, to_char(m.received_at,'YYYY-MM-DD HH24:MI:SS') "
            "AS sent, 'private' AS message_type FROM msgdata m JOIN users u ON m.sender_id = u.id JOIN users r ON m.recipient_id = r.id WHERE m.recipient_id = " + str + ") "
            "UNION ALL "
            "(SELECT u.login AS sender_login, r.login AS recipient_login, m.content, to_char(m.received_at,'YYYY-MM-DD HH24:MI:SS') "
            "AS sent, 'private' AS message_type FROM msgdata m JOIN users u ON m.sender_id = u.id JOIN users r ON m.recipient_id = r.id WHERE m.sender_id = " + str + " "
            "AND m.recipient_id IS NOT NULL) "
            "ORDER BY sent DESC";
    str = DBHandler::getConnection()->getData(query);
    return str.size();
}
