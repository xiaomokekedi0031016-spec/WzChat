#ifndef CHATVIEW_H
#define CHATVIEW_H

#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

/******************************************************************************
 *
 * @file       chatview.h
 * @brief      聊天窗口布局
 *
 * @author     badwoman
 * @date       2025/11/16
 * @history
 *****************************************************************************/

class ChatView: public QWidget
{
    Q_OBJECT
public:
    ChatView(QWidget *parent = Q_NULLPTR);
    /*
    appendChatItem：在尾部添加聊天消息（最新消息）
    prependChatItem：在头部插入（比如加载历史记录）
    insertChatItem：插入到指定消息之前（用于中间插入，比如系统提示）
*/
    void appendChatItem(QWidget *item);                 //尾插
    void prependChatItem(QWidget *item);                //头插
    void insertChatItem(QWidget *before, QWidget *item);//中间插
    void removeAllItem();
protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void paintEvent(QPaintEvent *event) override;
private slots:
    void onVScrollBarMoved(int min, int max);
private:
    void initStyleSheet();
private:
    //QWidget *m_pCenterWidget;
    QVBoxLayout *m_pVl;        // 存放所有聊天项的垂直布局
    QScrollArea *m_pScrollArea;// 滚动区域
    bool isAppended;// 用于判断是否是尾插(缓冲)
};
#endif // CHATVIEW_H
