#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "startscreen.h"
#include <QMessageBox>
#include <QTimer>
#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QListWidget>


MainWindow::MainWindow(int id, const QString& name, std::shared_ptr<Session> s, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , session(s)
    , userID(id)
    , username(name)
{
    ui->setupUi(this);
    updateChat();
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
    auto w = new MainWindow(s.getUserID(), s.getUsername(), s.getSession());
//  w->setAttribute(Qt::WA_DeleteOnClose);
    return w;
}

void MainWindow::on_msgEdit_returnPressed()
{
    on_pubButton_clicked();
}

void MainWindow::on_pubButton_clicked()
{
    QString str = "3#" + username + ' ' + ui->msgEdit->text();
    session->sendToServer(str);
    ui->msgEdit->clear();
    QTimer::singleShot(100, this, [=]() {
        auto respond = session->getBuffer();
        if(respond == "-1")
        {
            QMessageBox::critical(this, tr("error"), tr("failed dispatch"));
            return;
        }
        else
        {
            ui->textBrowser->clear();
            updateChat();
        }
    });
}
void MainWindow::on_pvtButton_clicked()
{
    QDialog dialog(this);
    dialog.setModal(true);
    auto l = new QVBoxLayout();
    dialog.setLayout(l);
    auto userListWgt = new QListWidget(&dialog);
    l->addWidget(userListWgt);
    auto bnbox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    l->addWidget(bnbox);
    connect(bnbox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(bnbox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    QString str{"4#"};
    session->sendToServer(str);
    str.clear();
    QTimer::singleShot(100, this, [=, &str]() {
        auto respond = session->getBuffer();
        for(auto c : respond)
        {
            if(c == '\n')
            {
                userListWgt->addItem(str);
                str.clear();
            }
            else
            {
                str.push_back(c);
            }
        }
    });
    userListWgt->setCurrentRow(0);
    auto result = dialog.exec();
    if(result == QDialog::Accepted && userListWgt->currentItem())
    {
        str = "3#" + username + " @" + userListWgt->currentItem()->text() + '@' + ui->msgEdit->text();
        session->sendToServer(str);
        ui->msgEdit->clear();
        QTimer::singleShot(100, this, [=]() {
            auto respond = session->getBuffer();
            if(respond == "-1")
            {
                QMessageBox::critical(this, tr("error"), tr("failed dispatch"));
                return;
            }
            else
            {
                ui->textBrowser->clear();
                updateChat();
            }
        });
    }
}

void MainWindow::updateChat()
{
    QString str = "6#" + QString::number(userID);
    session->sendToServer(str);
    QTimer::singleShot(100, this, [=]() {
        if(!session->check()) return;
        auto respond = session->getBuffer();
        if(respond == "-1")
        {
            QMessageBox::critical(this, tr("error"), tr("failed attempt to obtain data"));
            return;
        }
        else
        {
            QTextDocument *doc = ui->textBrowser->document();
            QTextCursor cursor(doc);
            QStringList parts = respond.split(' ');
            cursor.insertText(respond);
            QRegExp privateTag("private");
            QRegExp publicTag("public");
            QTextCharFormat privateFormat;
            privateFormat.setForeground(QColor(255, 0, 0));
            QTextCharFormat publicFormat;
            publicFormat.setForeground(QColor(0, 0, 255));
            cursor.setPosition(0);
            while (!cursor.atEnd())
            {
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
                if (privateTag.indexIn(cursor.selectedText()) != -1)
                {
                    cursor.insertText(cursor.selectedText(), privateFormat);
                }
                else if (publicTag.indexIn(cursor.selectedText()) != -1)
                {
                    cursor.insertText(cursor.selectedText(), publicFormat);
                }
            }
            ui->textBrowser->update();
        }
    });
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QString str = "5#" + QString::number(userID);
    session->sendToServer(str);
    qDebug() << str;
    QTimer::singleShot(0, this, [=]() {
        event->accept();
    });
}



