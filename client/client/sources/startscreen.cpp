#include "startscreen.h"
#include "ui_startscreen.h"

StartScreen::StartScreen(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StartScreen),
    session(std::make_shared<Session>())
{
    ui->setupUi(this);
    ui->regWidget->setSession(session);
    ui->loginWidget->setSession(session);
    connect(ui->loginWidget, &LoginForm::regRequested, this, &StartScreen::setRegForm);
    connect(ui->loginWidget, &LoginForm::accepted, this, &StartScreen::onLoggedIn);
    connect(ui->loginWidget, &LoginForm::rejected, this, &StartScreen::onRejectRequested);
    connect(ui->regWidget, &RegForm::loginRequested, this, &StartScreen::setLoginForm);
    connect(ui->regWidget, &RegForm::accepted, this, &StartScreen::onLoggedIn);
    connect(ui->regWidget, &RegForm::rejected, this, &StartScreen::onRejectRequested);
}

StartScreen::~StartScreen()
{
    delete ui;
}

void StartScreen::setLoginForm()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void StartScreen::setRegForm()
{
    ui->stackedWidget->setCurrentIndex(1);
}

std::shared_ptr<Session> StartScreen::getSession() const
{
    return session;
}

void StartScreen::onLoggedIn(int id)
{
    userID = id;
    accept();
}

void StartScreen::onRejectRequested()
{
    reject();
}

int StartScreen::getUserID() const
{
    return userID;
}


