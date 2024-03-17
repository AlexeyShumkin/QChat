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
        auto respond = session->getBuffer();
        if(respond == "-1")
        {
            QMessageBox::critical(this, tr("error"), tr("invalid login or password"));
        }
        else
        {
            auto id = respond.toInt();
            emit accepted(id, ui->loginEdit->text());
        }
    });
}


void LoginForm::on_buttonBox_rejected()
{
    emit rejected();
}

