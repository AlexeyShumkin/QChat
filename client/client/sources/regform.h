#ifndef REGFORM_H
#define REGFORM_H

#include <QWidget>
#include <memory>
#include "session.h"

namespace Ui {
class RegForm;
}

class RegForm : public QWidget
{
    Q_OBJECT

public:
    explicit RegForm(QWidget *parent = nullptr);
    ~RegForm();
    void setSession(std::shared_ptr<Session> s);

signals:
    void loginRequested();
    void accepted();
    void rejected();

private slots:
    void on_loginButton_clicked();
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::RegForm *ui;
    std::shared_ptr<Session> session;
};

#endif // REGFORM_H
