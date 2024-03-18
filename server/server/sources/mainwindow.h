#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "server.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void updateChat();
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_banButton_clicked();
    void on_disconnectButton_clicked();

private:
    Ui::MainWindow *ui;
    std::unique_ptr<Server> server;
};
#endif // MAINWINDOW_H
