#ifndef CHATITEMBASE_H
#define CHATITEMBASE_H

#include <QWidget>
#include <QGridLayout>//Qt的网格布局类
#include <QLabel>
#include "global.h"

class ChatItemBase : public QWidget
{
    Q_OBJECT
public:
    explicit ChatItemBase(ChatRole role, QWidget *parent = nullptr);
    void setUserName(const QString &name);
    void setUserIcon(const QPixmap &icon);
    void setWidget(QWidget *w);

private:
    ChatRole m_role;//聊天角色
    QLabel *m_pNameLabel;//nameLabel
    QLabel *m_pIconLabel;//iconLabel
    QWidget *m_pBubble;//气泡区域

};

#endif // CHATITEMBASE_H
