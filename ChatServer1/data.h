#pragma once
#include <string>

// 用户信息结构体
struct UserInfo {
	UserInfo() :name(""), pwd(""), uid(0), 
		email(""), nick(""), desc(""), 
		sex(0), icon(""), back("") {}
	std::string name;   // 用户名（登录名）
	std::string pwd;    // 密码（通常是加密后的）
	int uid;            // 用户唯一ID
	std::string email;  // 邮箱
	std::string nick;   // 昵称
	std::string desc;   // 个性签名 / 描述
	int sex;            // 性别（0/1/2）
	std::string icon;   // 头像URL或路径
	std::string back;   // 背景图
};

// 用户好友申请信息结构体
struct ApplyInfo {
	ApplyInfo(int uid, std::string name, std::string desc, std::string icon, std::string nick, int sex, int status)
		:_uid(uid), _name(name), _desc(desc),
		_icon(icon), _nick(nick), _sex(sex),
		_status(status) {}

	int _uid;           // 申请人 uid
	std::string _name;  // 申请人用户名
	std::string _desc;  // 申请说明
	std::string _icon;  // 头像
	std::string _nick;  // 昵称
	int _sex;           // 性别
	int _status;        // 申请状态（待处理/同意/拒绝）

};