#include "tcpmgr.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "userdata.h"
#include "usermgr.h"


TcpMgr::TcpMgr()
    :_host("")
    ,_port(0)
    ,_buffer("")
    ,_b_recv_pending(false)
    ,_message_id(0)
    ,_message_len(0){
    QObject::connect(&_socket,&QTcpSocket::connected,[&](){
        qDebug()<<"Connected to server!";
        emit sig_con_success(true);
    });
    //连接服务器失败,服务器断开连接,网络异常
    //QOverload处理重载信号
    QObject::connect(&_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
                     [&](QAbstractSocket::SocketError socketError){
        Q_UNUSED(socketError)
        qDebug() << "Error:" << _socket.errorString();
    });
    // 处理连接断开
    QObject::connect(&_socket, &QTcpSocket::disconnected, [&]() {
        qDebug() << "Disconnected from server.";
        //并且发送通知到界面
        //emit sig_connection_closed();
    });
    //连接发送信号用来发送数据
    QObject::connect(this, &TcpMgr::sig_send_data, this, &TcpMgr::slot_send_data);
    //数据可读就会触发(对端)
    QObject::connect(&_socket,&QTcpSocket::readyRead,[&](){
        qDebug()<<"Connected to server!";
        // 当有数据可读时，读取所有数据
        // 读取所有数据并追加到缓冲区
        _buffer.append(_socket.readAll());
        QDataStream stream(&_buffer, QIODevice::ReadOnly);
        stream.setVersion(QDataStream::Qt_5_0);
        forever{
            //qDebug()<<"6666666666666666";
            if(!_b_recv_pending){
                if(_buffer.size() < static_cast<int>(sizeof(quint16)*2)){
                    return;
                }
                //预读取消息ID和消息长度，但不从缓冲区中移除
                stream >> _message_id >> _message_len;
                _buffer = _buffer.mid(sizeof(quint16) * 2);
                qDebug() << "Message ID:" << _message_id
                         << ", Length:" << _message_len;
                if(_buffer.size() < _message_len){
                    _b_recv_pending = true;
                    return;
                }
                _b_recv_pending = false;
                QByteArray messageBody = _buffer.mid(0, _message_len);
                qDebug() << "receive body msg is " << messageBody ;
                _buffer = _buffer.mid(_message_len);
                //处理回调
                handleMsg(ReqId(_message_id),_message_len, messageBody);
            }
        }
    });

    initHandlers();
}

void TcpMgr::slot_tcp_connect(ServerInfo si)
{
    qDebug()<<"Connect to server...";
    _host = si.Host;
    _port = static_cast<uint16_t>(si.Port.toUInt());
    _socket.connectToHost(si.Host, _port);
}

void TcpMgr::slot_send_data(ReqId reqId, QByteArray dataBytes)
{
    qDebug()<<"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    uint16_t id = reqId;

    // 计算长度（使用网络字节序转换）
    quint16 len = static_cast<quint16>(dataBytes.length());

    // 创建一个QByteArray用于存储要发送的所有数据
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);//QDataStream用来按二进制格式写数据，写入到block
    //可以用 << 把各种数据按二进制顺序写进去

    // 设置数据流使用网络字节序
    out.setByteOrder(QDataStream::BigEndian);

    // 写入ID和长度
    out << id << len;//也写入了block

    // 添加字符串数据
    block.append(dataBytes);

    // 发送数据
    qDebug() << "[TcpMgr] send data id=" << reqId << "len=" << dataBytes.size() << "json=" << dataBytes;

    _socket.write(block);
    qDebug() << "tcp mgr send byte data is " << block ;
}

void TcpMgr::initHandlers()
{
    //qDebug()<<"55555555555555555";
    //登录聊天服务器回包
    _handlers.insert(ID_CHAT_LOGIN_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        //qDebug()<<"44444444444444444";
        qDebug()<< "handle id is "<< id <<" data is "<<data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.contains("error")){
           int err = ErrorCodes::ERR_JSON;
           qDebug() << "ID_CHAT_LOGIN_RSP Failed, err is Json Parse Err" << err ;
           return;
        }
        int err = jsonObj["error"].toInt();
        // if(err != ErrorCodes::SUCCESS){
        //     qDebug() << "Login Failed, err is " << err ;
        //     emit sig_login_failed(err);
        //     return;
        // }
        qDebug()<<"Login Chat Server Success!";
        auto uid = jsonObj["uid"].toInt();
        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto desc = jsonObj["desc"].toString();
        auto user_info = std::make_shared<UserInfo>(uid, name, nick, icon, sex, "", desc);
         qDebug() << "uid:" << uid << ", name:" << name << ", nick:" << nick
                  << ", icon:" << icon;
        UserMgr::GetInstance()->SetUserInfo(user_info);
        UserMgr::GetInstance()->SetToken(jsonObj["token"].toString());

        if(jsonObj.contains("apply_list")){
            UserMgr::GetInstance()->AppendApplyList(jsonObj["apply_list"].toArray());
        }

        //添加好友列表
        if (jsonObj.contains("friend_list")) {
            UserMgr::GetInstance()->AppendFriendList(jsonObj["friend_list"].toArray());
        }

        //聊天界面跳转
        emit sig_swich_chatdlg();

    });
    //搜索用户回包
    _handlers.insert(ID_SEARCH_USER_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug()<< "handle id is "<< id <<" data is "<<data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_SEARCH_USER_RSP Failed, err is Json Parse Err" << err ;
            emit sig_user_search(nullptr);
            return;
        }
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "ID_SEARCH_USER_RSP Failed, err is " << err ;
            emit sig_user_search(nullptr);
            return;
        }
        //搜索的用户信息 B
        auto search_info = std::make_shared<SearchInfo>(jsonObj["uid"].toInt(),
                                                        jsonObj["name"].toString(),
                                                        jsonObj["nick"].toString(),
                                                        jsonObj["desc"].toString(),
                                                        jsonObj["sex"].toInt(),
                                                        jsonObj["icon"].toString());
        emit sig_user_search(search_info);
    });
    //用户申请好友回包
    _handlers.insert(ID_ADD_FRIEND_RSP, [this](ReqId id, int len, QByteArray data){
        //qDebug()<<"222222222222222222222222";
        Q_UNUSED(len);
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        //qDebug()<< "data jsonobj is " << jsonObj ;

        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_ADD_FRIEND_RSP Failed, err is Json Parse Err" << err ;
            //emit sig_login_failed(err);
            return;
        }

        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "ID_ADD_FRIEND_RSP Failed, err is " << err ;
            //emit sig_login_failed(err);
            return;
        }
        qDebug()<<"Add Friend REQ Success!";

    });
    //通知用户添加好友申请回包(实际上是对端B执行的)
    _handlers.insert(ID_NOTIFY_ADD_FRIEND_REQ, [this](ReqId id, int len, QByteArray data){
        qDebug()<<"777777777777777777777";
        Q_UNUSED(len);
        //qDebug()<< "handle id is "<< id <<" data is "<<data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if(jsonDoc.isNull()){
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        //qDebug()<< "data jsonobj is " << jsonObj ;

        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "ID_NOTIFY_ADD_FRIEND_REQ Failed, err is Json Parse Err" << err ;
            return;
        }

        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "ID_NOTIFY_ADD_FRIEND_REQ Failed, err is " << err ;
            return;
        }
        qDebug()<<"ID_NOTIFY_ADD_FRIEND_REQ  Success!";
        //申请人的uid
        int from_uid = jsonObj["applyuid"].toInt();//A
        QString name = jsonObj["name"].toString();
        QString desc = jsonObj["desc"].toString();
        QString icon = jsonObj["icon"].toString();
        QString nick = jsonObj["nick"].toString();
        int sex = jsonObj["sex"].toInt();

        auto apply_info = std::make_shared<AddFriendApply>(from_uid, name, desc,
                                                           icon, nick, sex);
        emit sig_friend_apply(apply_info);
    });
    //认证好友回复 >> 做界面的ui Aerr   B
    _handlers.insert(ID_AUTH_FRIEND_RSP, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug()<< "handle id is "<< id <<" data is "<<data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        if(jsonDoc.isNull()){
            qDebug()<<"Failed to create QJsonDocument.";
            return;
        }
        QJsonObject jsonObj = jsonDoc.object();
        if(!jsonObj.contains("error")){
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Auth Friend Failed, err is Json Parse Err" << err;
            return;
        }
        int err = jsonObj["error"].toInt();
        if(err != ErrorCodes::SUCCESS){
            qDebug() << "Auth Friend Failed, err is " << err ;
            return;
        }
        auto name = jsonObj["name"].toString();
        auto nick = jsonObj["nick"].toString();
        auto icon = jsonObj["icon"].toString();
        auto sex = jsonObj["sex"].toInt();
        auto uid = jsonObj["uid"].toInt();//被认证的uid A
        auto rsp = std::make_shared<AuthRsp>(uid, name, nick, icon, sex);
        emit sig_auth_rsp(rsp);
        qDebug()<<"Auth Friend Success";
    });
    //通知用户有人添加好友进行好友认证(对端A)
    _handlers.insert(ID_NOTIFY_AUTH_FRIEND_REQ, [this](ReqId id, int len, QByteArray data){
        Q_UNUSED(len);
        qDebug()<<"handle id is "<<id<<" data is "<<data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);
        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();
        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Auth Friend Failed, err is " << err;
            return;
        }
        //申请认证的uid
        int from_uid = jsonObj["fromuid"].toInt();//B
        QString name = jsonObj["name"].toString();
        QString nick = jsonObj["nick"].toString();
        QString icon = jsonObj["icon"].toString();

        int sex = jsonObj["sex"].toInt();

        auto auth_info = std::make_shared<AuthInfo>(from_uid,name,
                                                    nick, icon, sex);
        //界面操作
        emit sig_add_auth_friend(auth_info);
    });
    //*******************************************************//
    //当前端的文本聊天消息回复(消息发送方)A
    _handlers.insert(ID_TEXT_CHAT_MSG_RSP, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Chat Msg Rsp Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Chat Msg Rsp Failed, err is " << err;
            return;
        }

        qDebug() << "Receive Text Chat Rsp Success " ;
        //ui设置送达等标记 todo...
    });
    //文本聊天消息通知(消息接收方)B
    _handlers.insert(ID_NOTIFY_TEXT_CHAT_MSG_REQ, [this](ReqId id, int len, QByteArray data) {
        Q_UNUSED(len);
        qDebug() << "handle id is " << id << " data is " << data;
        // 将QByteArray转换为QJsonDocument
        QJsonDocument jsonDoc = QJsonDocument::fromJson(data);

        // 检查转换是否成功
        if (jsonDoc.isNull()) {
            qDebug() << "Failed to create QJsonDocument.";
            return;
        }

        QJsonObject jsonObj = jsonDoc.object();

        if (!jsonObj.contains("error")) {
            int err = ErrorCodes::ERR_JSON;
            qDebug() << "Notify Chat Msg Failed, err is Json Parse Err" << err;
            return;
        }

        int err = jsonObj["error"].toInt();
        if (err != ErrorCodes::SUCCESS) {
            qDebug() << "Notify Chat Msg Failed, err is " << err;
            return;
        }

        qDebug() << "Receive Text Chat Notify Success " ;
        auto msg_ptr = std::make_shared<TextChatMsg>(jsonObj["fromuid"].toInt(),
                                                     jsonObj["touid"].toInt(),
                                                     jsonObj["text_array"].toArray());
        emit sig_text_chat_msg(msg_ptr);
    });

}


void TcpMgr::handleMsg(ReqId id, int len, QByteArray data)
{
    //qDebug()<<"77777777777777777";
    auto find_iter =  _handlers.find(id);
    if(find_iter == _handlers.end()){
        qDebug()<< "not found id ["<< id << "] to handle";
        return ;
    }

    find_iter.value()(id,len,data);
}

TcpMgr::~TcpMgr()
{

}

