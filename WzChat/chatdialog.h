#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "global.h"
#include "statewidget.h"
#include "userdata.h"

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();

protected:
    //事件过滤器
    bool eventFilter(QObject* watched, QEvent* event) override;
    //处理鼠标事件
    void handleGlobalMousePress(QMouseEvent *event);

private slots:
    //加载更多聊天信息
    void slot_loading_chat_user();
    //聊天侧边栏
    void slot_side_chat();
    //联系人侧边栏
    void slot_side_contact();
    //搜索栏变化
    void slot_text_changed(const QString &str);
    //加载更多联系人
    void slot_loading_contact_user();
    void slot_friend_info_page(std::shared_ptr<UserInfo> user_info);
    void slot_switch_apply_friend_page();

private:
    //展示QLineEdit三个其中一个
    void ShowSearch(bool bsearch = false);
    //添加13条测试记录
    void addChatUserList();
    //添加更多测试数据
    void loadMoreChatUser();
    //清除侧边栏的高亮状态
    void ClearLabelState(StateWidget *lb);
    //添加StateWidget侧边栏
    void AddLBGroup(StateWidget* lb);
    //添加更多联系人
    void loadMoreConUser();

private:
    Ui::ChatDialog *ui;
    ChatUIMode _mode;//当前模式
    ChatUIMode _state;//状态 >> 在其他模式下可以转换其它模式
    bool _b_loading;//类似于加载中的互斥锁
    //存储StateWidget侧边栏
    QList<StateWidget*> _lb_list;
    //最后一个widget的状态
    QWidget* _last_widget;
};

#endif // CHATDIALOG_H
