#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <memory>
#include "session.h"

namespace Ui {
class LoginForm;
}

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();
    void setSession(std::shared_ptr<Session> s);

signals:
    void regRequested();
    void accepted(int id);
    void rejected();

private slots:
    void on_regButton_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::LoginForm *ui;
    std::shared_ptr<Session> session;
};

#endif // LOGINFORM_H
