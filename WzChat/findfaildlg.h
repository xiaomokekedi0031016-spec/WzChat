#ifndef FINDFAILDLG_H
#define FINDFAILDLG_H

#include <QDialog>

/******************************************************************************
 *
 * @file       findfaildlg.h
 * @brief      搜索好友搜索失败的对话框
 *
 * @author     badwoman
 * @date       2025/12/21
 * @history
 *****************************************************************************/

namespace Ui {
class FindFailDlg;
}

class FindFailDlg : public QDialog
{
    Q_OBJECT

public:
    explicit FindFailDlg(QWidget *parent = nullptr);
    ~FindFailDlg();

private:
    Ui::FindFailDlg *ui;

private slots:
    void on_fail_sure_btn_clicked();
};

#endif // FINDFAILDLG_H
