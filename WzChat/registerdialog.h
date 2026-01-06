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

private:
    //显示错误信息
    void showTip(QString str, bool b_ok);
    //初始化回调函数
    void initHttpHandlers();

private:
    Ui::RegisterDialog *ui;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;

signals:
    void sigSwitchLogin();
};

#endif // REGISTERDIALOG_H
