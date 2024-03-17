#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QDialog>
#include "session.h"
#include <memory>

namespace Ui {
class StartScreen;
}

class StartScreen : public QDialog
{
    Q_OBJECT

public:
    explicit StartScreen(QWidget *parent = nullptr);
    ~StartScreen();
    void setLoginForm();
    void setRegForm();
    std::shared_ptr<Session> getSession() const;
    int getUserID() const;

    const QString &getUsername() const;

public slots:
    void onLoggedIn(int id, QString name);
    void onRejectRequested();
private:
    Ui::StartScreen *ui;
    std::shared_ptr<Session> session;
    int userID;
    QString username;
};

#endif // STARTSCREEN_H
