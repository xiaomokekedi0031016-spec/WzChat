#ifndef HTTPMGR_H
#define HTTPMGR_H
#include "singleton.h"
#include <QNetworkAccessManager>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include "global.h"

class HttpMgr : public QObject,
                public Singleton<HttpMgr>,
                public std::enable_shared_from_this<HttpMgr>
{
    Q_OBJECT
    friend class Singleton<HttpMgr>;
public:
    ~HttpMgr();
    //Post请求
    void PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod);

private:
    HttpMgr();

private:
    //Qt网络请求中枢
    QNetworkAccessManager _manager;

signals:
    //处理http发送完毕的信号
    void sig_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
    //处理注册模块http相关请求响应完成的槽函数
    void sig_reg_mod_finish(ReqId id, QString res, ErrorCodes err);
    //处理重置模块http相关请求响应完成的槽函数
    void sig_reset_mod_finish(ReqId id, QString res, ErrorCodes err);
    //处理登录模块http相关请求响应完成的槽函数
    void sig_login_mod_finish(ReqId id, QString res, ErrorCodes err);

private slots:
    //注册模块http发送完毕且接收响应后的槽函数
    void slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod);
};

#endif // HTTPMGR_H
