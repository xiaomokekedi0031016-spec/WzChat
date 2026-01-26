#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "global.h"
#include <QMap>

namespace Ui {
class RegisterDialog;
}

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    //点击注册界面的获取验证码按钮的槽函数
    void on_get_code_clicked();
    //根据id来处理对应的回调函数
    void slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    //点击注册界面的确定按钮槽函数(转到槽)
    void on_sure_btn_clicked();
    //点击注册界面的返回登录按钮槽函数
    void on_return_btn_clicked();
    //点击注册界面的取消按钮槽函数
    void on_cancel_btn_clicked();

private:
    //显示错误信息
    void showTip(QString str, bool b_ok);
    //初始化回调函数
    void initHttpHandlers();
    //注册界面的加载界面切换
    void ChangeTipPage();
    //检查输入内容是否合法
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkConfirmValid();
    bool checkVarifyValid();
    //添加错误提示
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);

private:
    Ui::RegisterDialog *ui;
    //回调操作
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    //错误输入 提示字符串
    QMap<TipErr, QString> _tip_errs;
    //定时器(用于注册完毕界面)
    QTimer* _countdown_timer;
    int _countdown;

signals:
    //切换到登录界面信号
    void sigSwitchLogin();
};

#endif // REGISTERDIALOG_H
