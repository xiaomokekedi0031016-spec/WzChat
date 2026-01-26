#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H
#include <QListWidget>
#include <QEvent>
#include <QWheelEvent>

class ChatUserList : public QListWidget
{
    Q_OBJECT
public:
    ChatUserList(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject* watched, QEvent* event)override;

private:
    bool _load_pending;

signals:
    // 信号通知加载更多聊天内容
    void sig_loading_chat_user();
};

#endif // CHATUSERLIST_H
