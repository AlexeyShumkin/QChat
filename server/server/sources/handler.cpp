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
    query = "SELECT login FROM users";
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

    query = "(select u.login, m.content, to_char(m.received_at,'YYYY-MM-DD HH24:MI:SS') as sent, "
                            "case when m.recipient_id is null then 'public' else 'private' end as message_type "
                            "from msgdata m join users u on m.sender_id = u.id where m.recipient_id IS NUll)"
                            "union all"
                            "(select u.login, m.content, to_char(m.received_at,'YYYY-MM-DD HH24:MI:SS') as sent, "
                            "case when m.recipient_id = " + str + " then 'private' else 'public' end as message_type "
                            "from msgdata m join users u on m.sender_id = u.id where m.recipient_id = " + str + ")"
                            "union all"
                            "(select u.login, m.content, to_char(m.received_at,'YYYY-MM-DD HH24:MI:SS') as sent, "
                            "case when m.sender_id = " + str + " and m.recipient_id is not null then 'private' else 'public' end as message_type "
                            "from msgdata m join users u on m.sender_id = u.id where m.sender_id = " + str + " and m.recipient_id is not null) "
                            "ORDER BY sent DESC";
        str = DBHandler::getConnection()->getData(query);
        return str.size();
}
