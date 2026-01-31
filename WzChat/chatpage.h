#ifndef CHATPAGE_H
#define CHATPAGE_H

#include "userdata.h"

#include <QWidget>

namespace Ui {
class ChatPage;
}

class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();
    //添加聊天记录
    void AppendChatMsg(std::shared_ptr<TextChatData> msg);
    //用户聊天界面
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);

protected:
    void paintEvent(QPaintEvent *event) override;

signals:
    void sig_append_send_chat_msg(std::shared_ptr<TextChatData> msg);

private slots:
    //聊天发送消息
    void on_send_btn_clicked();

private:
    Ui::ChatPage *ui;
    std::shared_ptr<UserInfo> _user_info;
};

#endif // CHATPAGE_H
