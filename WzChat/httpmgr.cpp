#include "httpmgr.h"
#include <QNetworkReply>

HttpMgr::~HttpMgr()
{

}

HttpMgr::HttpMgr()
{
    connect(this, &HttpMgr::sig_http_finish, this, &HttpMgr::slot_http_finish);
}

void HttpMgr::PostHttpReq(QUrl url, QJsonObject json, ReqId req_id, Modules mod)
{
    //HTTP版本号、Host、User-Agent这些Qt会自动加
    QByteArray data = QJsonDocument(json).toJson();
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, QByteArray::number(data.length()));
    auto self = shared_from_this();
    QNetworkReply * reply = _manager.post(request, data);
    QObject::connect(reply, &QNetworkReply::finished,[reply, self, req_id, mod](){
        if(reply->error() != QNetworkReply::NoError){
            qDebug()<<reply->errorString();//打印错误字符串
            emit self->sig_http_finish(req_id, "", ErrorCodes::ERR_NETWORK, mod);
            reply->deleteLater();//QNetworkReply不会自动释放，需要手动释放
            return;
        }

        QString res = reply->readAll();
        emit self->sig_http_finish(req_id, res, ErrorCodes::SUCCESS, mod);
        reply->deleteLater();
        return;
    });
}

void HttpMgr::slot_http_finish(ReqId id, QString res, ErrorCodes err, Modules mod)
{
    if(mod == Modules::REGISTERMOD){
        emit sig_reg_mod_finish(id, res, err);
    }
}

