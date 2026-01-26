#ifndef APPLYFRIEND_H
#define APPLYFRIEND_H

#include "friendlabel.h"
#include "userdata.h"

#include <QDialog>
#include <QMap>
#include "global.h"

namespace Ui {
class ApplyFriend;
}

class ApplyFriend : public QDialog
{
    Q_OBJECT

public:
    explicit ApplyFriend(QWidget *parent = nullptr);
    ~ApplyFriend();
    //初始化默认标签
    void InitTipLbs();
    //添加默认标签
    void AddTipLbs(class ClickedLabel*, QPoint cur_point, QPoint &next_point, int text_width, int text_height);
    //设置搜索信息(后期服务器)(前期虚拟数据)
    void SetSearchInfo(std::shared_ptr<SearchInfo> si);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

public slots:
    //显示更多label标签(more_lb)
    void ShowMoreLabel();
    //通过点击tip实现添加和减少好友便签
    void SlotChangeFriendLabelByTip(QString lbtext, ClickLbState state);
    //点击关闭，移除展示栏好友便签
    void SlotRemoveFriendLabel(QString name);
    //输入label按下回车触发将标签加入展示栏，并且隐藏输入框
    void SlotLabelEnter();
    //输入框文本变化显示不同提示
    void SlotLabelTextChange(const QString& text);
    //输入框输入完成
    void SlotLabelEditFinished();
    //输入标签显示提示框，点击提示框内容后添加好友便签
    void SlotAddFirendLabelByClickTip(QString text);
    //处理确认回调
    void SlotApplySure();
    //处理取消回调
    void SlotApplyCancel();

private:
    //添加标签到后面(新的/旧的)
    void addLabel(QString name);
    //重置label
    void resetLabels();

private:
    Ui::ApplyFriend *ui;
    //默认存储的所有标签
    std::vector<QString> _tip_data;
    //标签起始点(第一个)
    QPoint _tip_cur_point;
    //输入框中的起始点
    QPoint _label_point;
    //已经创建好的标签
    QMap<QString, ClickedLabel*> _add_labels;
    //存储标签的key
    std::vector<QString> _add_label_keys;
    //用户信息
    std::shared_ptr<SearchInfo> _si;
    //用来在输入框显示添加新好友的标签
    QMap<QString, FriendLabel*> _friend_labels;
    //存储friendlabel标签的key
    std::vector<QString> _friend_label_keys;
};

#endif // APPLYFRIEND_H
