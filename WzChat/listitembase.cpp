#include "listitembase.h"

#include <QPainter>
#include <QStyleOption>

ListItemBase::ListItemBase(QWidget *parent)
    :QWidget(parent)
{

}



void ListItemBase::SetItemType(ListItemType itemType)
{
    _itemType = itemType;
}



ListItemType ListItemBase::GetItemType()
{
    return _itemType;
}



void ListItemBase::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);                // 初始化样式选项，绑定当前控件信息

    QPainter p(this);              // 创建一个画家，用于在当前控件上绘制

    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    // 使用当前控件的样式对象来绘制控件的基本外观（背景、边框等）
}

