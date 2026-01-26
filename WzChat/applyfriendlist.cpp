#include "applyfriendlist.h"

#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>


ApplyFriendList::ApplyFriendList(QWidget *parent)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 安装事件过滤器
    this->viewport()->installEventFilter(this);
}



bool ApplyFriendList::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == this->viewport()){
        if(event->type() == QEvent::MouseButtonPress){
            emit sig_show_search(false);
        }else if(event->type() == QEvent::Enter){
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }else if(event->type() == QEvent::Leave){
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
    if(watched == this->viewport()){
        if(event->type() == QEvent::MouseButtonPress){
            emit sig_show_search(false);
        }
    }
    // 检查事件是否是鼠标滚轮事件
    if (watched == this->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15; // 计算滚动步数
        // 设置滚动幅度
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);
        return true; // 停止事件传递
    }

    return QListWidget::eventFilter(watched, event);
}

