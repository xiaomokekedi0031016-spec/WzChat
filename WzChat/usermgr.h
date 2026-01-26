#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include "singleton.h"
#include <memory>
#include "userdata.h"

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

private:
    UserMgr();
    //存储用户信息
    std::shared_ptr<UserInfo> _user_info;
    QString _token;
};

#endif // USERMGR_H
