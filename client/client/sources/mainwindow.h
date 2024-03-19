#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "session.h"
#include <memory>
#include <QCloseEvent>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int id, const QString& username, std::shared_ptr<Session> s = nullptr, QWidget *parent = nullptr);
    ~MainWindow();
    static MainWindow* createClient();

private slots:
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_pubButton_clicked();
    void on_msgEdit_returnPressed();
    void on_pvtButton_clicked();
    void closeEvent(QCloseEvent* event);
private:
    void updateChat();
    void updateUserList();
    Ui::MainWindow* ui;
    std::shared_ptr<Session> session;
    int userID;
    QString username;
};
#endif // MAINWINDOW_H
