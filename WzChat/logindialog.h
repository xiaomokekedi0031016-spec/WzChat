#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "global.h"
#include <QDialog>
#include <QMap>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

signals:
    //登录界面切换为注册界面
    void switchRegister();
    //登录界面切换为重置密码界面
    void switchReset();
    //发送信号连接tcp
    void sig_connect_tcp(ServerInfo si);

private slots:
    //点击忘记密码按钮(发送信号给MainWindow)
    void slot_forget_pwd();
    //点击登录按钮
    void on_login_btn_clicked();
    //处理登录模块http请求响应完成的槽函数
    void slot_login_mod_finish(ReqId id, QString res, ErrorCodes err);
    //tcp连接完成后的槽函数
    void slot_tcp_con_finish(bool bsuccess);
    //登录失败后的槽函数
    void slot_login_failed(int err);

private:
    Ui::LoginDialog *ui;
    QMap<TipErr, QString> _tip_errs;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    int _uid;
    QString _token;

private:
    //初始化登录界面的ui
    void initHead();
    //校验信息
    bool checkUserValid();
    bool checkPwdValid();
    void AddTipErr(TipErr type, QString msg);
    void DelTipErr(TipErr te);
    void showTip(QString str, bool b_ok);
    bool enableBtn(bool enabled);
    void initHttpHandlers();

};

#endif // LOGINDIALOG_H
