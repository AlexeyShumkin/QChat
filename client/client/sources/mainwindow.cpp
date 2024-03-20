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
    connect(session.get(), &Session::signalForButtons, this, &MainWindow::on_signalClientResponse);
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

void MainWindow::on_signalClientResponse(QString& respond)
{
    if(respond == "you are blocked")
    {
        ui->textBrowser->setText(respond);
        ui->pubButton->setEnabled(false);
        pvtButton->setEnabled(false);
    }
    else if(respond == "you are unblocked")
    {
        ui->textBrowser->setText(respond);
        ui->pubButton->setEnabled(true);
        pvtButton->setEnabled(true);
    }
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QDialog* option = new QDialog(this);
    option->setWindowTitle("options");
    QVBoxLayout* layout = new QVBoxLayout(option);
    QPushButton* pvtButton = new QPushButton("private message", option);
    connect(pvtButton, &QPushButton::clicked, this, &MainWindow::on_pvtButton_clicked);
    layout->addWidget(pvtButton);
    option->exec();
}

void MainWindow::on_msgEdit_returnPressed()
{
    on_pubButton_clicked();
}

void MainWindow::on_pubButton_clicked()
{
    if(session->socketBufferBlocked()) return;
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
    if(session->socketBufferBlocked()) return;
    QString recipient = ui->listWidget->currentItem()->text();
    recipient = recipient.mid(recipient.indexOf(' ') + 1, recipient.lastIndexOf(' ') - recipient.indexOf(' ') - 1);
    QString str = "3#" + username + " @" + recipient + '@' + ui->msgEdit->text();
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

void MainWindow::updateChat()
{
    if(session->socketBufferBlocked()) return;
    QString str = "6#" + QString::number(userID);
    session->clearBuffer();
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
            QTextDocument* doc = ui->textBrowser->document();
            QTextCursor cursor(doc);
            cursor.insertText(respond);
            QStringList lines = respond.split('\n');
            for(int i = 0; i < lines.size() - 1; ++i)
            {
                QString tmp = lines[i].mid(lines[i].indexOf(' ') + 1);
                tmp = tmp.mid(0, tmp.indexOf(' '));
                lines[i].insert(lines[i].indexOf(' ') + 1, "->");
                lines[i].insert(lines[i].indexOf('>') + tmp.size() + 1, ": ");
            }
            respond = lines.join('\n');
            ui->textBrowser->setText(respond);
            QRegExp privateTag("<pvt>");
            QRegExp publicTag("<pub>");
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
        updateUserList();
    });

}

void MainWindow::updateUserList()
{
    if(session->socketBufferBlocked()) return;
    ui->listWidget->clear();
    QString str{"4#"};
    session->clearBuffer();
    session->sendToServer(str);
    QTimer::singleShot(100, this, [=]() {
        auto respond = session->getBuffer();
        if(respond == "-1") return;
        QStringList lines = respond.split('\n');
        for(int i = 0; i < lines.size() - 1; ++i)
        {
            QString num = QString::number(i + 1);
            num += ") ";
            lines[i].prepend(num);
            if(lines[i].indexOf("online") >= 0)
            {
                QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
                item->setText(lines[i]);
                item->setForeground(QBrush(Qt::darkGreen));
            }
            else
            {
                QListWidgetItem *item = new QListWidgetItem(ui->listWidget);
                item->setText(lines[i]);
                item->setForeground(QBrush(Qt::darkGray));
            }
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



