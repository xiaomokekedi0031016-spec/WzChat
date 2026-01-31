#ifndef TCPMGR_H
#define TCPMGR_H
#include <QObject>
#include <QTcpSocket>
#include "singleton.h"
#include "global.h"
#include <QMap>
#include "userdata.h"

class TcpMgr : public QObject, public Singleton<TcpMgr>,
               public std::enable_shared_from_this<TcpMgr>
{
    Q_OBJECT
    friend class Singleton<TcpMgr>;
public:
    ~TcpMgr();

private:
    QTcpSocket _socket;
    QString _host;
    uint16_t _port;
    QByteArray _buffer;
    bool _b_recv_pending;
    quint16 _message_id;
    quint16 _message_len;//2字节
    QMap<ReqId, std::function<void(ReqId id, int len, QByteArray data)>> _handlers;

private:
    TcpMgr();
    void initHandlers();
    void handleMsg(ReqId id, int len, QByteArray data);

public slots:
    //连接服务器槽函数
    void slot_tcp_connect(ServerInfo si);
    //发送数据信号
    void slot_send_data(ReqId reqId, QByteArray dataBytes);

signals:
    //连接Tcp成功信号
    void sig_con_success(bool bsuccess);
    //发送数据可能出现的动画
    void sig_send_data(ReqId reqId, QByteArray data);
    //登录失败信号
    void sig_login_failed(int);
    //登录界面切换到聊天界面信号
    void sig_swich_chatdlg();
    //连接搜索条目
    void sig_user_search(std::shared_ptr<SearchInfo> si);
    //添加好友认证信号
    void sig_friend_apply(std::shared_ptr<AddFriendApply>);
    //认证好友回复信号
    void sig_auth_rsp(std::shared_ptr<AuthRsp>);
    //同意认证后的信号
    void sig_add_auth_friend(std::shared_ptr<AuthInfo>);
    //文本消息信号
    void sig_text_chat_msg(std::shared_ptr<TextChatMsg> msg);
};

#endif // TCPMGR_H
