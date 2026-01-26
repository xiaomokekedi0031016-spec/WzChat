#ifndef TEXTBUBBLE_H
#define TEXTBUBBLE_H

#include <QTextEdit>//文本框
#include "BubbleFrame.h"
#include <QHBoxLayout>

class TextBubble : public BubbleFrame
{
    Q_OBJECT
public:
    TextBubble(ChatRole role, const QString &text, QWidget *parent = nullptr);
protected:
    //调整文本的高度
    bool eventFilter(QObject *o, QEvent *e);
private:
    //调整文本高度
    void adjustTextHeight();
    //设置文本内容
    void setPlainText(const QString &text);
    void initStyleSheet();
private:
    QTextEdit *m_pTextEdit;
};

#endif // TEXTBUBBLE_H
