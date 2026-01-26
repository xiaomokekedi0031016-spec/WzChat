#include "clickedoncelabel.h"

ClickedOnceLabel::ClickedOnceLabel(QWidget *parent)
{
    setCursor(Qt::PointingHandCursor);
}



void ClickedOnceLabel::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked(this->text());
        return;
    }
        QLabel::mouseReleaseEvent(event);
}

