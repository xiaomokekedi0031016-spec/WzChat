#pragma once
#include "const.h"
#include "MysqlDao.h"
//mysql管理者
//数据访问的总入口
class MysqlMgr : public Singleton<MysqlMgr>
{
    friend class Singleton<MysqlMgr>;
public:
    ~MysqlMgr();
    int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
    bool CheckEmail(const std::string& name, const std::string& email);
    bool UpdatePwd(const std::string& name, const std::string& pwd);
    bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo);
    //bool TestProcedure(const std::string& email, int& uid, string& name);
    bool AddFriendApply(const int& from, const int& to);
    std::shared_ptr<UserInfo> GetUser(int uid);
    std::shared_ptr<UserInfo> GetUser(std::string name);
    //获取申请列表
    bool GetApplyList(int touid, std::vector<std::shared_ptr<ApplyInfo>>& applyList, int begin, int limit = 10);
    ////获取好友列表
    bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list);
    //bool AuthFriendApply(const int& from, const int& to);
    bool AddFriend(const int& from, const int& to, std::string back_name);
    bool AuthFriendApply(const int& from, const int& to);

private:
    MysqlMgr();
    MysqlDao _dao;
};