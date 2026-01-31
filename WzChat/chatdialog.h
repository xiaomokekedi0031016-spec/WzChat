#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
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
    //更新当前聊天界面记录
    void UpdateChatMsg(std::vector<std::shared_ptr<TextChatData>> msgdata);

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
    //切换到好友信息页面
    void slot_friend_info_page(std::shared_ptr<UserInfo> user_info);
    //切换到好友申请页面
    void slot_switch_apply_friend_page();

public slots:
    //申请添加好友槽函数
    void slot_apply_friend(std::shared_ptr<AddFriendApply> apply);
    //添加认证好友槽函数
    void slot_add_auth_friend(std::shared_ptr<AuthInfo> auth_info);
    //自己认证回复槽函数
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);
    //跳转聊天条目槽函数
    void slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg);
    //聊天列表点击槽函数
    void slot_item_clicked(QListWidgetItem* item);
    //连接搜索列表跳转聊天槽函数
    void slot_jump_chat_item(std::shared_ptr<SearchInfo> si);
    //连接好友信息页面跳转聊天槽函数
    void slot_jump_chat_item_from_infopage(std::shared_ptr<UserInfo> ui);
    //发送聊天消息槽函数
    void slot_append_send_chat_msg(std::shared_ptr<TextChatData> msgdata);

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
    //设置选中的聊天条目
    void SetSelectChatItem(int uid = 0);
    //设置选中的聊天页面
    void SetSelectChatPage(int uid = 0);

private:
    Ui::ChatDialog *ui;
    ChatUIMode _mode;//当前模式
    ChatUIMode _state;//状态 >> 在其他模式下可以转换其它模式
    bool _b_loading;//类似于加载中的互斥锁
    //存储StateWidget侧边栏
    QList<StateWidget*> _lb_list;
    //最后一个widget的状态
    QWidget* _last_widget;
    QMap<int, QListWidgetItem*> _chat_items_added;
    int _cur_chat_uid;//当前聊天用户ID
};

#endif // CHATDIALOG_H
