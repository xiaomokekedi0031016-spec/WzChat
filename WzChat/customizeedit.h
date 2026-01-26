#ifndef CUSTOMIZEEDIT_H
#define CUSTOMIZEEDIT_H

#include <QLineEdit>
#include <QDebug>

class CustomizeEdit : public QLineEdit
{
    Q_OBJECT
public:
    CustomizeEdit(QWidget *parent = nullptr);
    //设置最大输入长度
    void SetMaxLength(int maxLen);

protected:
    //失去焦点
    void focusOutEvent(QFocusEvent *event) override
    {
        //失去焦点:鼠标点到别的地方去了
        // 执行失去焦点时的处理逻辑
        //qDebug() << "CustomizeEdit focusout";
        // 调用基类的focusOutEvent()方法，保证基类的行为得到执行
        //子类如果覆盖事件必须调用基类，否则会破坏链
        QLineEdit::focusOutEvent(event);
        //发送失去焦点得信号
        emit sig_foucus_out();
    }

private:
    void limitTextLength(QString text);

private:
    int _max_len;

signals:
    void sig_foucus_out();

};

#endif // CUSTOMIZEEDIT_H
