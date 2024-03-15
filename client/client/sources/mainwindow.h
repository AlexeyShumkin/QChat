#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "session.h"
#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(std::shared_ptr<Session> s = nullptr, QWidget *parent = nullptr);
    ~MainWindow();
    static MainWindow* createClient();

private slots:
    void on_pubButton_clicked();
    void on_msgEdit_returnPressed();
    void on_pvtButton_clicked();

private:
    Ui::MainWindow* ui;
    std::shared_ptr<Session> session;
};
#endif // MAINWINDOW_H
