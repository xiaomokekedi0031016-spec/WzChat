#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H

#include <QLabel>
#include "global.h"

class ClickedLabel : public QLabel
{
    Q_OBJECT
public:
    ClickedLabel(QWidget* parent = nullptr);
    void SetState(QString normal="", QString hover="", QString press="",
              QString select="", QString select_hover="", QString select_press="");

    ClickLbState GetCurState();
    ~ClickedLabel();
    //设置当前状态
    bool SetCurState(ClickLbState state);
    //重置正常状态
    void ResetNormalState();

protected:
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent* ev)override;
    virtual void enterEvent(QEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;

private:
    //眼睛的select和normal状态
    ClickLbState _curstate;
    //控件各个状态
    QString _normal;
    QString _normal_hover;
    QString _normal_press;
    QString _selected;
    QString _selected_hover;
    QString _selected_press;


signals:
    void clicked(QString, ClickLbState);

};

#endif // CLICKEDLABEL_H
