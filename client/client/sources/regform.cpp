#include "regform.h"
#include "ui_regform.h"
#include <QMessageBox>
#include <QTimer>

RegForm::RegForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RegForm)
{
    ui->setupUi(this);
}

RegForm::~RegForm()
{
    delete ui;
}

void RegForm::setSession(std::shared_ptr<Session> s)
{
    session = s;
}

void RegForm::on_loginButton_clicked()
{
    emit loginRequested();
}

void RegForm::on_buttonBox_accepted()
{
    if(ui->passwordEdit->text() != ui->confirmPasswordEdit->text())
    {
        QMessageBox::critical(this, tr("error"), tr("passwords not match"));
        return;
    }
    std::hash<QString> hasher;
    QString str = "1#" + ui->loginEdit->text() + ' ' + QString::number(hasher(ui->passwordEdit->text()));
    session->sendToServer(str);
    QTimer::singleShot(1000, this, [=]() {
        auto respond = session->getBuffer();
        if(respond == "-1")
        {
            QMessageBox::critical(this, tr("error"), tr("login already exists"));
        }
        else
        {
            auto id = respond.toInt();
            emit accepted(id, ui->loginEdit->text());
        }
    });
}

void RegForm::on_buttonBox_rejected()
{
    emit rejected();
}

