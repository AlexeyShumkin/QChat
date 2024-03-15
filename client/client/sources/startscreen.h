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
public slots:
    void onLoggedIn();
    void onRejectRequested();
private:
    Ui::StartScreen *ui;
    std::shared_ptr<Session> session;
};

#endif // STARTSCREEN_H
