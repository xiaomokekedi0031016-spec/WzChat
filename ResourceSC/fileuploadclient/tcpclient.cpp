#include "tcpclient.h"
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonDocument>
#include "global.h"
#include <QFile>
#include <QCryptographicHash>

TcpClient::~TcpClient()
{

}

TcpClient::TcpClient(QObject *parent) : QObject(parent),
    _socket(new QTcpSocket(this)),_bytes_sent(0),_pending(false)
{
    //连接 连接成功信号和槽函数
    connect(_socket, &QTcpSocket::connected, this, &TcpClient::slot_connected);
    //连接 读数据信号和槽函数
    connect(_socket, &QTcpSocket::readyRead, this, &TcpClient::slot_ready_read);
    //连接 断开连接信号和槽函数
    connect(_socket, &QTcpSocket::disconnected, this, &TcpClient::slot_disconnected);
    //连接 出错信号和槽函数
    connect(_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
                    this, &TcpClient::slot_error_occured);
    //5.15之后的高版本才有的信号
//    connect(_socket, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::errorOccurred),
    //                this, &TcpClient::slot_error_occured);

    //连接 发送数据信号和槽函数
    connect(this, &TcpClient::sig_send_msg, this, &TcpClient::slot_send_msg);

    //连接信号
    connect(_socket, &QTcpSocket::bytesWritten, this, [this](qint64 bytes){
        //更新发送数据
        _bytes_sent += bytes;
        //未发完整
        if(_bytes_sent < _current_block.size()){
            auto data_to_send = _current_block.mid(_bytes_sent);
            _socket->write(data_to_send);
            return;
        }

        //发送完整
        if(_send_queue.isEmpty()){
            _current_block.clear();
            _pending = false;
            _bytes_sent = 0;
            return;
        }

        //队列不为空，则取出队首元素
        _current_block = _send_queue.dequeue();
        _bytes_sent = 0;
        _pending = true;
        _socket->write(_current_block);
    });
}

void TcpClient::processData()
{
    while(_buffer.size() >= TCP_HEAD_LEN){
        //先取出六字节头部
        auto head_byte = _buffer.left(TCP_HEAD_LEN);
        QDataStream stream(head_byte);
        //设置为大端模式
        stream.setByteOrder(QDataStream::BigEndian);
        //读取ID
        quint16 msg_id;
        stream >> msg_id;
        //读取长度
        quint32 body_length;
        stream >> body_length;
        if(_buffer.size() >= TCP_HEAD_LEN+body_length){
            //完整的消息体已经接受
            QByteArray body = _buffer.mid(TCP_HEAD_LEN,body_length);
            //去掉完整的消息包
            _buffer = _buffer.mid(TCP_HEAD_LEN+body_length);
            // 解析服务器发过来的消息
            QJsonDocument jsonDoc = QJsonDocument::fromJson(body);
            if(jsonDoc.isNull()){
                qDebug() << "Failed to create JSON doc.";
                this->_socket->close();
                return;
            }

            if(!jsonDoc.isObject()){
                qDebug() << "JSON is not an object.";
                this->_socket->close();
                return;
            }
            //qDebug() << "receive data is " << body;
            // 获取 JSON 对象
            QJsonObject jsonObject = jsonDoc.object();
            emit sig_logic_process(msg_id, jsonObject);
        }else{
            //消息未完全接受，所以中断
            break;
        }
    }
}

//发送数据槽函数
void TcpClient::slot_send_msg(quint16 id, QByteArray body)
{
    //如果连接异常则直接返回
    if(_socket->state() != QAbstractSocket::ConnectedState){
        emit sig_net_error(QString("断开连接无法发送"));
        return;
    }

    //获取body的长度
    quint32 bodyLength = body.size();

    //创建字节数组
    QByteArray data;
    //绑定字节数组
    QDataStream stream(&data, QIODevice::WriteOnly);
    //设置大端模式
    stream.setByteOrder(QDataStream::BigEndian);
    //写入ID
    stream << id;
    //写入长度
    stream << bodyLength;
    //写入包体
    data.append(body);
    if(_pending){
        _send_queue.enqueue(data);
        return;
    }

    // 没有正在发送，把这包设为“当前块”，重置计数，并写出去
    _current_block = data;        // ← 保存当前正在发送的 block
    _bytes_sent = 0;            // ← 归零
    _pending = true;         // ← 标记正在发送
    //发送消息
     _socket->write(data);
}

void TcpClient::connectToServer(const QString &host, quint16 port)
{
    _socket->connectToHost(host, port);
}

//断开连接函数
void TcpClient::disconnectFromServer()
{
    if(_socket->state() != QAbstractSocket::ConnectedState){
        return;
    }

    _socket->disconnectFromHost(); // 优雅地关闭连接

    // 或者使用 socket->close(); 立即关闭连接
    if (_socket->waitForDisconnected(3000)) {

    } else {
        emit sig_net_error(QString("关闭错误%1").arg(_socket->errorString()));
    }
}

void TcpClient::sendMsg(quint16 id,QByteArray data)
{
    //发送信号，统一交给槽函数处理，这么做的好处是多线程安全
    emit sig_send_msg(id, data);
}

bool TcpClient::isConnected()
{
    return _socket->state() == QAbstractSocket::ConnectedState;
}


//连接成功槽函数
void TcpClient::slot_connected()
{
    //重置按钮
    emit sig_connected(true);
}

//接受服务器发送的信息
void TcpClient::slot_ready_read()
{
    //读取所有数据
    QByteArray data = _socket->readAll();

    //将数据缓存起来
    _buffer.append(data);

    //处理收到的数据
    processData();
}

//捕获连接断开信号
void TcpClient::slot_disconnected()
{
    //连接断开,发送信号通知主界面弹窗
    emit sig_net_error(QString("连接断开"));
}

void TcpClient::slot_error_occured(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    //产生错误, 发送主界面弹窗
    emit sig_net_error(QString("网络错误: %1 ！").arg(_socket->errorString()));
    //出错则判断连接状态并重置
    if(_socket->state() != QAbstractSocket::ConnectedState){
        return;
    }

    //出错就关闭连接
    _socket->close();
}

