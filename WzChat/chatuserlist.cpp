#include "chatuserlist.h"
#include <QScrollBar>


ChatUserList::ChatUserList(QWidget *parent)
    :QListWidget(parent)
    ,_load_pending(false)
{
    Q_UNUSED(parent);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->viewport()->installEventFilter(this);
}


bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    //watched是被监视的对象，event是发生的事件
    if(watched == this->viewport()){//viewport表示真正显示内容的区域
        if (event->type() == QEvent::Enter) {
            // 鼠标悬浮，显示滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        } else if (event->type() == QEvent::Leave) {
            // 鼠标离开，隐藏滚动条
            this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }
    //滚轮事件
    if(watched == this->viewport() && event->type() == QEvent::Wheel){
        //向上滚一格+120 向下滚一个-120
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);
        int numDegrees = wheelEvent->angleDelta().y() / 8;
        int numSteps = numDegrees / 15;
        this->verticalScrollBar()->setValue(this->verticalScrollBar()->value() - numSteps);
        //判断是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        int maxScrollValue = scrollBar->maximum();
        int currentValue = scrollBar->value();
        if (maxScrollValue - currentValue <= 0) {
            // 滚动到底部，加载新的联系人
            //qDebug()<<"load more chat user";
            //发送信号通知聊天界面加载更多聊天内容
            emit sig_loading_chat_user();
        }

        return true; // 停止事件传递
    }
    return QListWidget::eventFilter(watched, event);
}




