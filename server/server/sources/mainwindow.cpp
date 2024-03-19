#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSqlQuery>
#include <QDialog>
#include <QPushButton>
#include <QListWidgetItem>
#include <QLineEdit>
#include <QLabel>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , server(std::make_unique<Server>())
{
    ui->setupUi(this);
    updateChat();
    connect(server.get(), &Server::clientResponse, this, &MainWindow::updateChat);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateChat()
{
    ui->textBrowser->clear();
    QString str = "SELECT u.login as sender_login, "
                  "CASE WHEN m.recipient_id IS NULL THEN 'all' ELSE r.login END "
                  "as recipient_login, m.content, to_char(m.received_at,'YYYY-MM-DD HH24:MI:SS') as sent, "
                  "CASE WHEN m.recipient_id IS NULL THEN '<pub>' ELSE '<pvt>' END as message_type "
                  "FROM msgdata m JOIN users u ON m.sender_id = u.id LEFT JOIN users r ON m.recipient_id = r.id "
                  "ORDER BY sent DESC";
    str = DBHandler::getConnection()->getData(str);
    QTextDocument* doc = ui->textBrowser->document();
    QTextCursor cursor(doc);
    cursor.insertText(str);
    QStringList lines = str.split('\n');
    for(int i = 0; i < lines.size() - 1; ++i)
    {
        QString tmp = lines[i].mid(lines[i].indexOf(' ') + 1);
        tmp = tmp.mid(0, tmp.indexOf(' '));
        lines[i].insert(lines[i].indexOf(' ') + 1, "->");
        lines[i].insert(lines[i].indexOf('>') + tmp.size() + 1, ": ");
    }
    str = lines.join('\n');
    ui->textBrowser->setText(str);
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
    updateUserList();
}

void MainWindow::updateUserList()
{
    ui->listWidget->clear();
    QString str = "SELECT id, login, status FROM users ORDER BY id DESC";
    str = DBHandler::getConnection()->getData(str);
    QStringList lines = str.split('\n');
    for(int i = 0; i < lines.size() - 1; ++i)
    {
        lines[i].prepend("id:");
        lines[i].insert(lines[i].indexOf(' ') + 1, " user:  ");
        lines[i].insert(lines[i].lastIndexOf(' ') + 1, "  ");
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
    ui->listWidget->setCurrentRow(0);
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    targetID = item->text();
    targetID = targetID.mid(targetID.indexOf(':') + 1, targetID.indexOf(' ') - targetID.indexOf(':') - 1);
    QDialog* options = new QDialog(this);
    options->setWindowTitle("options");
    QVBoxLayout* mainLayout = new QVBoxLayout(options);
    QHBoxLayout* topLayout = new QHBoxLayout();
    QLabel* label = new QLabel("block on seconds:");
    timerValue = new QLineEdit();
    QPushButton* banButton = new QPushButton("block");
    topLayout->addWidget(label);
    topLayout->addWidget(timerValue);
    topLayout->addWidget(banButton);
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    QPushButton* disconnectButton = new QPushButton("disconnect");
    bottomLayout->addWidget(disconnectButton);
    connect(banButton, &QPushButton::clicked, this, &MainWindow::on_banButton_clicked);
    connect(disconnectButton, &QPushButton::clicked, this, &MainWindow::on_disconnectButton_clicked);
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bottomLayout);
    options->exec();
}

void MainWindow::on_banButton_clicked()
{
    server->checkForBlock(targetID.toInt());
    int value = timerValue->text().toInt();
    timer = new QTimer(this);
    timer->setInterval(value * 1000);
    connect(timer, &QTimer::timeout, this, &MainWindow::unblock);
    timer->start();
}

void MainWindow::on_disconnectButton_clicked()
{
    server->disconnect(targetID.toInt());
    QSqlQuery query(DBHandler::getConnection()->getDb());
    query.prepare("UPDATE users SET status = 'offline' WHERE id = ?");
    query.addBindValue(targetID.toInt());
    if(!query.exec())
    {
        qDebug() << "error query";
    }
    updateUserList();
}

void MainWindow::unblock()
{
    server->checkForUnBlock(targetID.toInt());
    timer->stop();
}

void MainWindow::on_timerValue_returnPressed()
{
    on_banButton_clicked();
}
