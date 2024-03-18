#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QSqlQuery>
#include <QDialog>
#include <QPushButton>
#include <QListWidgetItem>

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
                  "CASE WHEN m.recipient_id IS NULL THEN 'public' ELSE 'private' END as message_type "
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
    ui->listWidget->clear();
    str = "SELECT id, login, status FROM users ORDER BY id DESC";
    str = DBHandler::getConnection()->getData(str);
    lines = str.split('\n');
    for(int i = 0; i < lines.size() - 1; ++i)
    {
        lines[i].prepend("id:");
        lines[i].insert(lines[i].indexOf(' ') + 1, " user:  ");
        lines[i].insert(lines[i].lastIndexOf(' ') + 1, "\t");
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
    QString id = item->text();
    id = id.mid(0, id.indexOf(' '));
    QDialog* options = new QDialog(this);
    options->setWindowTitle("options");
    QVBoxLayout *layout = new QVBoxLayout(options);
    QPushButton* banButton = new QPushButton("ban", options);
    QPushButton* disconnectButton = new QPushButton("disconnect", options);
    connect(banButton, &QPushButton::clicked, this, &MainWindow::on_banButton_clicked);
    connect(disconnectButton, &QPushButton::clicked, this, &MainWindow::on_disconnectButton_clicked);
    layout->addWidget(banButton);
    layout->addWidget(disconnectButton);
    options->exec();

}

void MainWindow::on_banButton_clicked()
{
    qDebug() << "BAAAAAN!";
}

void MainWindow::on_disconnectButton_clicked()
{
    qDebug() << "GOODBYE!";
}
