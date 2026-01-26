#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "logindialog.h"
#include "registerdialog.h"
#include "resetdialog.h"
#include "chatdialog.h"

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
    //切换为重置密码界面
    void SlotSwitchReset();
    //切换为登录界面
    void SlotSwitchLogin2();
    //跳转到聊天界面的槽函数
    void SlotSwitchChat();

private:
    Ui::MainWindow *ui;
    //登录窗口
    LoginDialog* _login_dlg;
    //注册窗口
    RegisterDialog* _reg_dlg;
    //重置窗口
    ResetDialog* _reset_dlg;
    //聊天界面
    ChatDialog* _chat_dlg;
};
#endif // MAINWINDOW_H
