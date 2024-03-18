#include "mainwindow.h"
#include "./ui_mainwindow.h"

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
    QTextDocument *doc = ui->textBrowser->document();
    QTextCursor cursor(doc);
    cursor.insertText(str);
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
