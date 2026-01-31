#ifndef CONTACTUSERLIST_H
#define CONTACTUSERLIST_H

#include "conuseritem.h"

#include <QListWidget>

class ContactUserList : public QListWidget
{
    Q_OBJECT
public:
    ContactUserList(QWidget *parent = nullptr);
    void ShowRedPoint(bool bshow = true);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override ;

private:
    //添加固有属性
    void addContactUserList();

public slots:
    //item点击槽函数
    void slot_item_clicked(QListWidgetItem *item);
    //对于对端添加到联系人列表
    void slot_add_auth_firend(std::shared_ptr<AuthInfo>);
    //对于本端添加到联系人列表
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);
signals:
    //加载更多联系人
    void sig_loading_contact_user();
    //跳转好友申请界面
    void sig_switch_apply_friend_page();
    //跳转联系人界面
    void sig_switch_friend_info_page(std::shared_ptr<UserInfo> user_info);
private:
    ConUserItem* _add_friend_item;
    QListWidgetItem * _groupitem;
};
#endif // CONTACTUSERLIST_H
