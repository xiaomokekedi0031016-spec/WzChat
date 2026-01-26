#ifndef APPLYFRIENDITEM_H
#define APPLYFRIENDITEM_H

#include "listitembase.h"
#include "userdata.h"

#include <QWidget>

namespace Ui {
class ApplyFriendItem;
}

class ApplyFriendItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ApplyFriendItem(QWidget *parent = nullptr);
    ~ApplyFriendItem();
    void SetInfo(std::shared_ptr<ApplyInfo> apply_info);
    void ShowAddBtn(bool bshow);
    QSize sizeHint() const override {
        return QSize(250, 80); // 返回自定义的尺寸
    }
    int GetUid();

signals:
    //审核好友信号
    void sig_auth_friend(std::shared_ptr<ApplyInfo> apply_info);

private:
    Ui::ApplyFriendItem *ui;
    //好友申请信息
    std::shared_ptr<ApplyInfo> _apply_info;
    //是否已添加
    bool _added;
};

#endif // APPLYFRIENDITEM_H
