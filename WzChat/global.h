#ifndef GLOBAL_H
#define GLOBAL_H
#pragma once
#include <functional>
#include <QWidget>
#include <QStyle>

//刷新qss
extern std::function<void(QWidget*)> repolish;
extern std::function<QString(QString)> xorString;

extern QString gate_url_prefix;

//请求的枚举id
enum ReqId
{
    ID_GET_VARIFY_CODE = 1001,//获取验证码
    ID_REG_USER = 1002,//注册用户
    ID_RESET_PWD = 1003, //重置密码
    ID_LOGIN_USER = 1004, //用户登录
    ID_CHAT_LOGIN = 1005, //登陆聊天服务器
    ID_CHAT_LOGIN_RSP= 1006, //登陆聊天服务器回包
    ID_SEARCH_USER_REQ = 1007, //用户搜索请求
    ID_SEARCH_USER_RSP = 1008, //搜索用户回包
    ID_ADD_FRIEND_REQ = 1009,  //添加好友申请
    ID_ADD_FRIEND_RSP = 1010, //申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,  //通知用户添加好友申请
    ID_AUTH_FRIEND_REQ = 1013,  //认证好友请求
    ID_AUTH_FRIEND_RSP = 1014,  //认证好友回复
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015, //通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ  = 1017,  //文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP  = 1018,  //文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, //通知用户文本聊天信息
    ID_NOTIFY_OFF_LINE_REQ = 1021, //通知用户下线
    ID_HEART_BEAT_REQ = 1023,      //心跳请求
    ID_HEARTBEAT_RSP = 1024,       //心跳回复
};

//请求的模块枚举(哪个模块发送的请求)
enum Modules{
    REGISTERMOD = 0,//注册模块
    RESETMOD = 1,//重置密码模块
    LOGINMOD = 2,//登录模块
};

//请求错误码
enum ErrorCodes{
    SUCCESS = 0,//成功
    ERR_JSON = 1,//JSON解析失败
    ERR_NETWORK = 2,//网络错误
};

//注册信息错误提示枚举
enum TipErr {
    TIP_SUCCESS = 0,      // 成功，无错误
    TIP_EMAIL_ERR = 1,    // 邮箱格式或邮箱相关错误
    TIP_PWD_ERR = 2,      // 密码错误
    TIP_CONFIRM_ERR = 3,  // 确认输入错误（可能是表单确认）
    TIP_PWD_CONFIRM = 4,  // 密码与确认密码不匹配
    TIP_VARIFY_ERR = 5,   // 验证码或验证错误
    TIP_USER_ERR = 6      // 用户名或用户相关错误
};

//聊天消息文本
const std::vector<QString>  strs ={"hello world !",
                                   "nice to meet u",
                                   "New year，new life",
                                   "You have to love yourself",
                                   "My love is written in the wind ever since the whole world is you"};

//头像
const std::vector<QString> heads = {
    ":/res/head_1.jpg",
    ":/res/head_2.jpg",
    ":/res/head_3.jpg",
    ":/res/head_4.jpg",
    ":/res/head_5.jpg"
};
//昵称
const std::vector<QString> names = {
    "HanMeiMei",
    "Lily",
    "Ben",
    "Androw",
    "Max",
    "Summer",
    "Candy",
    "Hunter"
};
#endif // GLOBAL_H
