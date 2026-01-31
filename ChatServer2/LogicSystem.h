#pragma once
#include "Singleton.h"
#include "CSession.h"
#include "data.h"

using FunCallBack = std::function<void(std::shared_ptr<CSession>, const short& msg_id, const string& msg_data)>;

class LogicSystem : public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
public:
	~LogicSystem();
	void PostMsgToQue(shared_ptr <LogicNode> msg);

private:
	LogicSystem();
	//注册回调
	void RegisterCallBacks();
	//线程处理函数
	void DealMsg();
	//处理登录回包
	void LoginHandler(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	//搜索好友回包
	void SearchInfo(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	//添加好友确认回包(USER_BASE_INFO是对端的信息)
	void AddFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	//认证好友回包
	void AuthFriendApply(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);
	//聊天消息回包
	void DealChatTextMsg(std::shared_ptr<CSession> session, const short& msg_id, const string& msg_data);

	//存储(获取)用户信息
	bool GetBaseInfo(std::string base_key, int uid, std::shared_ptr<UserInfo>& userinfo);
	//判断是qt的lineEdit输入的是uid还是name
	bool isPureDigit(const std::string& str);
	//根据uid获取信息
	void GetUserByUid(std::string uid_str, Json::Value& rtvalue);
	//根据name获取信息
	void GetUserByName(std::string name, Json::Value& rtvalue);
	//获取申请好友列表信息
	bool GetFriendApplyInfo(int to_uid, std::vector<std::shared_ptr<ApplyInfo>>& list);
	//获取好友列表
	bool GetFriendList(int self_id, std::vector<std::shared_ptr<UserInfo>>& user_list);

private:
	bool _b_stop;//服务器关闭true
	std::thread _worker_thread;//工作线程
	std::queue<shared_ptr<LogicNode>> _msg_que;//消息队列
	std::mutex _mutex;//队列锁
	std::condition_variable _consume;//条件变量	
	std::map<short, FunCallBack> _fun_callbacks;//消息id到处理函数的映射
};