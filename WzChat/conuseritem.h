#ifndef CONUSERITEM_H
#define CONUSERITEM_H

#include <QWidget>
#include "listitembase.h"
#include "userdata.h"

namespace Ui {
class ConUserItem;
}


class ConUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ConUserItem(QWidget *parent = nullptr);
    ~ConUserItem();
    //返回默认的大小
    QSize sizeHint() const override;
    //设置信息
    void SetInfo(std::shared_ptr<AuthInfo> auth_info);
    //void SetInfo(std::shared_ptr<AuthRsp> auth_rsp);
    void SetInfo(int uid, QString name, QString icon);
    void ShowRedPoint(bool show = false);
    std::shared_ptr<UserInfo> GetInfo();
private:
    Ui::ConUserItem *ui;
    std::shared_ptr<UserInfo> _info;
};

#endif // CONUSERITEM_H
