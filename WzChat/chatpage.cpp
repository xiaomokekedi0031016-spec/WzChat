#include "chatitembase.h"
#include "chatpage.h"
#include "picturebubble.h"
#include "textbubble.h"
#include "ui_chatpage.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QStyleOption>
#include "chatview.h"
#include "global.h"
#include "tcpmgr.h"
#include "usermgr.h"
#include <qDebug>


ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatPage)
{
    ui->setupUi(this);
    ui->receive_btn->SetState("normal","hover","press");
    ui->send_btn->SetState("normal","hover","press");
    ui->emo_lb->SetState("normal","hover","press","normal","hover","press");
    ui->file_lb->SetState("normal","hover","press","normal","hover","press");
}

ChatPage::~ChatPage()
{
    delete ui;
}


void ChatPage::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);

    QPainter p(this);

    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


void ChatPage::on_send_btn_clicked()
{
    //如果没有选择聊天对象则不发送消息
    if (_user_info == nullptr) {
        qDebug() << "friend_info is empty";
        return;
    }

    auto user_info = UserMgr::GetInstance()->GetUserInfo();
    auto pTextEdit = ui->chatEdit;
    ChatRole role = ChatRole::Self;
    QString userName = user_info->_name;
    QString userIcon = user_info->_icon;

    const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
    QJsonObject textObj;
    QJsonArray textArray;
    int txt_size = 0;

    for(int i=0; i<msgList.size(); ++i)
    {
        //消息内容长度不合规就跳过
        if(msgList[i].content.length() > 1024){
            continue;
        }

        QString type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(userName);
        pChatItem->setUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;

        if(type == "text")
        {
            //生成唯一id（消息id)
            QUuid uuid = QUuid::createUuid();
            //转为字符串
            QString uuidString = uuid.toString();

            pBubble = new TextBubble(role, msgList[i].content);
            //如果再加一条消息超过了1024字节，就先发送之前累计的消息
            if(txt_size + msgList[i].content.length()> 1024){
                textObj["fromuid"] = user_info->_uid;//A
                textObj["touid"] = _user_info->_uid;//B
                textObj["text_array"] = textArray;
                QJsonDocument doc(textObj);
                QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
                //发送并清空之前累计的文本列表
                txt_size = 0;
                textArray = QJsonArray();
                textObj = QJsonObject();
                //发送tcp请求给chat server
                emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
            }

            //将bubble和uid绑定，以后可以等网络返回消息后设置是否送达
            //_bubble_map[uuidString] = pBubble;
            txt_size += msgList[i].content.length();
            QJsonObject obj;
            QByteArray utf8Message = msgList[i].content.toUtf8();
            obj["content"] = QString::fromUtf8(utf8Message);
            obj["msgid"] = uuidString;
            textArray.append(obj);
            auto txt_msg = std::make_shared<TextChatData>(uuidString,
                                                          obj["content"].toString(),
                                                          user_info->_uid,
                                                          _user_info->_uid);
            emit sig_append_send_chat_msg(txt_msg);
        }
        else if(type == "image")
        {
            pBubble = new PictureBubble(QPixmap(msgList[i].content) , role);
        }
        else if(type == "file")
        {

        }
        //发送消息
        if(pBubble != nullptr)
        {
            pChatItem->setWidget(pBubble);
            ui->chat_data_list->appendChatItem(pChatItem);
        }

    }

    //最后一条消息的发送
    qDebug() << "textArray is " << textArray ;
    //发送给服务器
    textObj["text_array"] = textArray;
    textObj["fromuid"] = user_info->_uid;
    textObj["touid"] = _user_info->_uid;
    QJsonDocument doc(textObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);
    //发送并清空之前累计的文本列表
    txt_size = 0;
    textArray = QJsonArray();
    textObj = QJsonObject();
    //发送tcp请求给chat server
    emit TcpMgr::GetInstance()->sig_send_data(ReqId::ID_TEXT_CHAT_MSG_REQ, jsonData);
}

// void ChatPage::on_send_btn_clicked(){
//     auto pTextEdit = ui->chatEdit;
//     ChatRole role = ChatRole::Self;
//     QString userName = QStringLiteral("badwoman");
//     QString userIcon = ":/res/head_1.jpg";

//     const QVector<MsgInfo>& msgList = pTextEdit->getMsgList();
//     for(int i=0; i<msgList.size(); ++i)
//     {
//         QString type = msgList[i].msgFlag;
//         ChatItemBase *pChatItem = new ChatItemBase(role);
//         pChatItem->setUserName(userName);
//         pChatItem->setUserIcon(QPixmap(userIcon));
//         QWidget *pBubble = nullptr;
//         if(type == "text")
//         {
//             pBubble = new TextBubble(role, msgList[i].content);
//             // qDebug() << "pTextEdit:" << pTextEdit;
//             // qDebug() << "msgList size:" << msgList.size();

//         }
//         else if(type == "image")
//         {
//             pBubble = new PictureBubble(QPixmap(msgList[i].content) , role);
//         }
//         else if(type == "file")
//         {

//         }
//         if(pBubble != nullptr)
//         {
//             pChatItem->setWidget(pBubble);
//             ui->chat_data_list->appendChatItem(pChatItem);//这里实际上隐含父子关系了
//         }
//     }
// }


void ChatPage::AppendChatMsg(std::shared_ptr<TextChatData> msg)
{
    auto self_info = UserMgr::GetInstance()->GetUserInfo();
    ChatRole role;
    //添加聊天显示
    if(msg->_from_uid == self_info->_uid){
        role = ChatRole::Self;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(self_info->_name);
        pChatItem->setUserIcon(QPixmap(self_info->_icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }else
    {
        role = ChatRole::Other;
        ChatItemBase* pChatItem = new ChatItemBase(role);
        pChatItem->setUserName(_user_info->_name);
        pChatItem->setUserIcon(QPixmap(_user_info->_icon));
        QWidget* pBubble = nullptr;
        pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->setWidget(pBubble);
        ui->chat_data_list->appendChatItem(pChatItem);
    }
}



void ChatPage::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
    //设置ui界面
    ui->title_lb->setText(_user_info->_name);
    ui->chat_data_list->removeAllItem();
    for(auto & msg : user_info->_chat_msgs){
        AppendChatMsg(msg);
    }
}

