#ifndef SEARCHLIST_H
#define SEARCHLIST_H

#include "findsuccessdlg.h"
#include "loadingdlg.h"

#include <QListWidget>


class SearchList : public QListWidget
{
public:
    SearchList(QWidget *parent = nullptr);
    //设置搜索窗口用于获取文字
    void SetSearchEdit(QWidget* edit);
    void CloseFindDlg();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    //点击条目的槽函数(查找uid/name)
    void slot_item_clicked(QListWidgetItem *item);
    //搜索结果返回槽函数
    void slot_user_search(std::shared_ptr<SearchInfo> si);

private:
    //添加提示条目
    void addTipItem();
    //等待加载中对话框
    void waitPending(bool pending = true);

private:
    //搜索框查找成功结果
    FindSuccessDlg* m_findSuccessDlg;//测试用
    //搜索框
    std::shared_ptr<QDialog> _find_dlg;
    //控制单一搜索请求 >> 不能让它有其它界面的交互
    bool _send_pending;
    //加载动画
    LoadingDlg * _loadingDialog;
    //搜索编辑框
    QWidget* _search_edit;

};

#endif // SEARCHLIST_H
