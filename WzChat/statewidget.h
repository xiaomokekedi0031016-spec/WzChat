#ifndef STATEWIDGET_H
#define STATEWIDGET_H

#include <QLabel>
#include <QWidget>
#include "global.h"

class StateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit StateWidget(QWidget *parent = nullptr);

    void SetState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");

    ClickLbState GetCurState();
    void ClearState();

    void SetSelected(bool bselected);
    //红点提示收到了消息
    void AddRedPoint();
    void ShowRedPoint(bool show=true);

protected:
    void paintEvent(QPaintEvent* event);

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void enterEvent(QEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

private:

    QString _normal;
    QString _normal_hover;
    QString _normal_press;

    QString _selected;
    QString _selected_hover;
    QString _selected_press;
    //控件状态
    ClickLbState _curstate;
    //红点label
    QLabel * _red_point;

signals:
    void clicked(void);


};

#endif // STATEWIDGET_H
