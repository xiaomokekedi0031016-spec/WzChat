#pragma once

#include<QString>

//TCP包头长度
#define TCP_HEAD_LEN 6
//TCP ID长度
#define TCP_ID_LEN 2
//TCP 长度字段的长度
#define TCP_LEN_LEN 4
//最大文件长度
#define MAX_FILE_LEN 2048

// 初始发送窗口大小（客户端启动时一次性并发发送多少个块）
#define INIT_SEND_WINDOW 4
// 最大拥塞窗口（防止无限增长）
#define MAX_CWND 64


enum MsgId {
    ID_TEST_MSG_REQ = 1001,       //测试消息
    ID_TEST_MSG_RSP = 1002,       //测试消息回包
    ID_UPLOAD_FILE_REQ = 1003,    //发送文件请求
    ID_UPLOAD_FILE_RSP = 1004,    //发送文件回复
    ID_SYNC_FILE_REQ = 1005,      //同步文件请求
    ID_SYNC_FILE_RSP = 1006,      //同步文件回复
};

enum RspErr {
    RSP_SUCCESS = 0,          //正常回包
    RSP_INVALID_ID = 1,       //消息id不正确
};

extern QString generateUniqueId();
