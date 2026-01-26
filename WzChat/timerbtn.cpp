#include "timerbtn.h"

#include <QMouseEvent>

TimerBtn::TimerBtn(QWidget* parent)
    :QPushButton(parent)
    ,_counter(10)
{
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, [this](){
        _counter--;
        if(_counter <= 0){
            _timer->stop();
            _counter = 10;
            this->setText("获取");
            //控制控件是否可用
            this->setEnabled(true);
            return;
        }
        this->setText(QString::number(_counter));
    });
}

void TimerBtn::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton) {
        // 在这里处理鼠标左键释放事件
        this->setEnabled(false);
        this->setText(QString::number(_counter));
        _timer->start(1000);
        emit clicked();
    }
    QPushButton::mouseReleaseEvent(e);
}


TimerBtn::~TimerBtn()
{
    _timer->stop();
}

