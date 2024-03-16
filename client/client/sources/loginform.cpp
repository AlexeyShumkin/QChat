#include "loginform.h"
#include "ui_loginform.h"
#include <QMessageBox>
#include <QTimer>

LoginForm::LoginForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginForm)
{
    ui->setupUi(this);
}

LoginForm::~LoginForm()
{
    delete ui;
}

void LoginForm::setSession(std::shared_ptr<Session> s)
{
    session = s;
}

void LoginForm::on_regButton_clicked()
{
    emit regRequested();
}


void LoginForm::on_buttonBox_accepted()
{
    std::hash<QString> hasher;
    QString str = "2#" + ui->loginEdit->text() + ' ' + QString::number(hasher(ui->passwordEdit->text()));
    session->sendToServer(str);
    QTimer::singleShot(1000, this, [=]() {
        auto i = session->getBuffer().toInt();
        switch(i)
        {
        case -1:
            QMessageBox::critical(this, tr("error"), tr("Invalid login or password"));
            return;
        default:
            emit accepted();
            return;
        }
    });
}


void LoginForm::on_buttonBox_rejected()
{
    emit rejected();
}

