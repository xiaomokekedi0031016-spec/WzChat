#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registerdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    //切换为注册界面
    void SlotSwitchReg();
    //切换为登录界面
    void SlotSwitchLogin();

private:
    Ui::MainWindow *ui;
    //登录窗口
    LoginDialog* _login_dlg;
    //注册窗口
    RegisterDialog* _reg_dlg;
};
#endif // MAINWINDOW_H
