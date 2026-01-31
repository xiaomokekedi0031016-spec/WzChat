#pragma once
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>
#include <memory>
#include <iostream>
#include "Singleton.h"
#include <unordered_map>
#include <hiredis.h>
#include <json/value.h>
#include <json/json.h>
#include <json/reader.h>
#include <queue>
#include <atomic>
#include <string>

namespace net = boost::asio;
namespace beast = boost::beast;
namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

//错误码
enum ErrorCodes {
	Success = 0,
	Error_Json = 1001,  //Json解析错误
	RPCFailed = 1002,  //RPC请求错误
	VarifyExpired = 1003, //验证码过期
	VarifyCodeErr = 1004, //验证码错误
	UserExist = 1005,       //用户已经存在
	PasswdErr = 1006,    //密码错误
	EmailNotMatch = 1007,  //邮箱不匹配
	PasswdUpFailed = 1008,  //更新密码失败
	PasswdInvalid = 1009,   //密码更新失败
	TokenInvalid = 1010,   //Token失效
	UidInvalid = 1011,  //uid无效
};

class Defer {
public:
	Defer(std::function<void()> func) : func_(func) {}
	~Defer() {
		func_();
	}

private:
	std::function<void()> func_;
};

#define CODEPREFIX "code_"//redis的key前缀 >> 用户通过redis获取校验服务器存储的验证码
#define USERIPPREFIX  "uip_"//当前所在的chatServer
#define USERTOKENPREFIX  "utoken_"//将token存储到redis服务器里
#define IPCOUNTPREFIX  "ipcount_"
#define USER_BASE_INFO "ubaseinfo_"//用户基本信息前缀
#define LOGIN_COUNT  "logincount"//获取服务器对应的连接数
#define NAME_INFO  "nameinfo_"//用户名到uid的映射
#define LOCK_PREFIX "lock_"
#define USER_SESSION_PREFIX "usession_"//用户session前缀
#define LOCK_COUNT "lockcount"

#define MAX_LENGTH  1024*2//最大消息长度
#define HEAD_TOTAL_LEN 4//头部总长度
#define HEAD_ID_LEN 2//头部消息ID长度
#define HEAD_DATA_LEN 2//头部数据长度
#define MAX_SENDQUE 1000//发送队列最大长度

enum MSG_IDS {
	MSG_CHAT_LOGIN = 1005, //用户登陆
	MSG_CHAT_LOGIN_RSP = 1006, //用户登陆回包
	ID_SEARCH_USER_REQ = 1007, //用户搜索请求
	ID_SEARCH_USER_RSP = 1008, //搜索用户回包
	ID_ADD_FRIEND_REQ = 1009, //申请添加好友请求
	ID_ADD_FRIEND_RSP = 1010, //申请添加好友回复
	ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //通知用户添加好友申请
	ID_AUTH_FRIEND_REQ = 1013,  //认证好友请求
	ID_AUTH_FRIEND_RSP = 1014,  //认证好友回复
	ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //通知用户认证好友申请
	ID_TEXT_CHAT_MSG_REQ = 1017, //文本聊天信息请求
	ID_TEXT_CHAT_MSG_RSP = 1018, //文本聊天信息回复
	ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //通知用户文本聊天信息
	ID_NOTIFY_OFF_LINE_REQ = 1021, //通知用户下线
	ID_HEART_BEAT_REQ = 1023,      //心跳请求
	ID_HEARTBEAT_RSP = 1024,       //心跳回复
};