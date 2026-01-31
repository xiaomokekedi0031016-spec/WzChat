#ifndef APPLYFRIENDPAGE_H
#define APPLYFRIENDPAGE_H

#include <QWidget>
#include "userdata.h"
#include <memory>
#include <QJsonArray>
#include <unordered_map>
#include "applyfrienditem.h"

namespace Ui {
class ApplyFriendPage;
}

class ApplyFriendPage : public QWidget
{
    Q_OBJECT

public:
    explicit ApplyFriendPage(QWidget *parent = nullptr);
    ~ApplyFriendPage();
    //添加新的好友信息
    void AddNewApply(std::shared_ptr<AddFriendApply> apply);
protected:
    void paintEvent(QPaintEvent *event);
private:
    //加载好友申请列表
    void loadApplyList();
    Ui::ApplyFriendPage *ui;
    //存储未认证的好友申请项
    std::unordered_map<int, ApplyFriendItem*> _unauth_items;
public slots:
    //更新好友申请列表(认证过了)
    void slot_auth_rsp(std::shared_ptr<AuthRsp>);
signals:
    void sig_show_search(bool);
};

#endif // APPLYFRIENDPAGE_H
