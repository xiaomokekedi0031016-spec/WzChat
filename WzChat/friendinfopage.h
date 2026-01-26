#ifndef FRIENDINFOPAGE_H
#define FRIENDINFOPAGE_H

#include "userdata.h"

#include <QWidget>

namespace Ui {
class FriendInfoPage;
}

class FriendInfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit FriendInfoPage(QWidget *parent = nullptr);
    ~FriendInfoPage();
    void SetInfo(std::shared_ptr<UserInfo> user_info);

private slots:
    //聊天按钮
    void on_msg_chat_clicked();

signals:
    //切换到聊天页面
    void sig_jump_chat_item(std::shared_ptr<UserInfo> si);

private:
    Ui::FriendInfoPage *ui;
    //存储用户信息
    std::shared_ptr<UserInfo> _user_info;
};

#endif // FRIENDINFOPAGE_H
