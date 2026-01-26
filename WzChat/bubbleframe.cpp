#include "bubbleframe.h"

#include <QPainter>

const int WIDTH_SANJIAO  = 8;  //三角宽
BubbleFrame::BubbleFrame(ChatRole role, QWidget *parent)
    :QFrame(parent)
    ,m_role(role)
    ,m_margin(3)
{
    m_pHLayout = new QHBoxLayout();
    //目的是给聊天气泡留出三角形的空间
    if(m_role == ChatRole::Self)
        m_pHLayout->setContentsMargins(m_margin, m_margin, WIDTH_SANJIAO + m_margin, m_margin);
    else
        m_pHLayout->setContentsMargins(WIDTH_SANJIAO + m_margin, m_margin, m_margin, m_margin);

    this->setLayout(m_pHLayout);
}

void BubbleFrame::setMargin(int margin)
{
    Q_UNUSED(margin);
    //m_margin = margin;
}

//气泡里面的widget
void BubbleFrame::setWidget(QWidget *w)
{
    //这里的count是统计布局里面的控件数的
    if(m_pHLayout->count() > 0)
        return ;
    else{
        m_pHLayout->addWidget(w);
    }
}

void BubbleFrame::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);//不带边框

    if(m_role == ChatRole::Other)
    {
        //画气泡
        QColor bk_color(Qt::white);
        painter.setBrush(QBrush(bk_color));//画刷
        QRect bk_rect = QRect(WIDTH_SANJIAO, 0, this->width()-WIDTH_SANJIAO, this->height());
        painter.drawRoundedRect(bk_rect,5,5);//5, 5：分别为 x方向圆角半径 和 y方向圆角半径
        //画小三角
        QPointF points[3] = {
            QPointF(bk_rect.x(), 12),
            QPointF(bk_rect.x(), 10+WIDTH_SANJIAO +2),
            QPointF(bk_rect.x()-WIDTH_SANJIAO, 10+WIDTH_SANJIAO-WIDTH_SANJIAO/2),
        };
        painter.drawPolygon(points, 3);
    }
    else
    {
        QColor bk_color(158,234,106);
        //QBrush填充类，setBrush设置填充颜色
        painter.setBrush(QBrush(bk_color));
        //画气泡，QRect矩阵类
        QRect bk_rect = QRect(0, 0, this->width()-WIDTH_SANJIAO, this->height());
        painter.drawRoundedRect(bk_rect,5,5);
        //画三角
        QPointF points[3] = {
            //参数横坐标和纵坐标
            QPointF(bk_rect.x()+bk_rect.width(), 12),
            QPointF(bk_rect.x()+bk_rect.width(), 12+WIDTH_SANJIAO +2),
            QPointF(bk_rect.x()+bk_rect.width()+WIDTH_SANJIAO, 10+WIDTH_SANJIAO-WIDTH_SANJIAO/2),
        };
        painter.drawPolygon(points, 3);

    }

    return QFrame::paintEvent(e);
}
