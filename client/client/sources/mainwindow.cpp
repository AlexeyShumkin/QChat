#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "startscreen.h"

MainWindow::MainWindow(int id, std::shared_ptr<Session> s, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , userID(id)
{
    ui->setupUi(this);
    session = s;
}

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow *MainWindow::createClient()
{
    StartScreen s;
    auto result = s.exec();
    if(result == QDialog::Rejected)
    {
        return nullptr;
    }
    auto w = new MainWindow(s.getUserID(), s.getSession());
    w->setAttribute(Qt::WA_DeleteOnClose);
    return w;
}

void MainWindow::on_msgEdit_returnPressed()
{
    on_pubButton_clicked();
}

void MainWindow::on_pubButton_clicked()
{
    QString str = "3#" + QVariant(userID).toString() + ui->msgEdit->text();
    session->sendToServer(str);
}
void MainWindow::on_pvtButton_clicked()
{

}



