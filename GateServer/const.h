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

//redis的key前缀 >> 用户通过redis获取校验服务器存储的验证码
#define CODEPREFIX "code_"	

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