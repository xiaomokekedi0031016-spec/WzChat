#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include "singleton.h"
#include <memory>
#include "userdata.h"
#include <QMap>

class UserMgr : public QObject, public Singleton<UserMgr>,
                public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT

public:
    friend class Singleton<UserMgr>;
    ~UserMgr();
    void SetToken(QString token);
    //登录时候调用注册信息
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);
    //获取uid
    int GetUid();
    //获取name
    QString GetName();
    //添加申请信息
    void AddApplyList(std::shared_ptr<ApplyInfo> app);
    //防止重复添加好友
    bool AlreadyApply(int uid);
    //自己认证别人
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    //对方收到
    void AddFriend(std::shared_ptr<AuthInfo> auth_info);
    //判断是否已经是好友
    bool CheckFriendById(int uid);
    //获取申请信息
    std::vector<std::shared_ptr<ApplyInfo>> GetApplyList();
    //添加申请列表
    void AppendApplyList(QJsonArray array);
    //添加好友列表
    void AppendFriendList(QJsonArray array);
    //获取用户信息
    std::shared_ptr<UserInfo> GetUserInfo();
    //获取聊天记录列表中一页的信息
    std::vector<std::shared_ptr<FriendInfo>> GetChatListPerPage();
    //更新已经加载的条目
    void UpdateChatLoadedCount();
    //初始化FriendInfo中的消息
    void AppendFriendChatMsg(int friend_id, std::vector<std::shared_ptr<TextChatData>>);
    //获取好友信息
    std::shared_ptr<FriendInfo> GetFriendById(int uid);

private:
    UserMgr();
    //存储用户信息
    std::shared_ptr<UserInfo> _user_info;
    QString _token;
    //存储申请信息
    std::vector<std::shared_ptr<ApplyInfo>> _apply_list;
    //存储好友
    QMap<int, std::shared_ptr<FriendInfo>> _friend_map;
    //聊天列表加载计数
    std::vector<std::shared_ptr<FriendInfo>> _friend_list;
    int _chat_loaded;//聊天加载的数量
    int _contact_loaded;//联系人加载的数量
};

#endif // USERMGR_H
