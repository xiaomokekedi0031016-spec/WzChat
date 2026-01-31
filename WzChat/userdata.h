#ifndef USERDATA_H
#define USERDATA_H
#include <QJsonArray>
#include <QString>
#include <memory>
#include <QJsonObject>


//搜索信息结构体
class SearchInfo {
public:
    SearchInfo(int uid, QString name, QString nick,
               QString desc, int sex, QString icon)
        :_uid(uid)
        ,_name(name)
        ,_nick(nick)
        ,_desc(desc)
        ,_sex(sex)
        ,_icon(icon){}

    int _uid;//用户id
    QString _name;//用户名
    QString _nick;//昵称
    QString _desc;//描述
    int _sex;//性别
    QString _icon;//头像
};


//好友认证结构体
struct AuthInfo {
    AuthInfo(int uid, QString name, QString nick,
             QString icon, int sex)
        :_uid(uid)
        , _name(name)
        , _nick(nick)
        , _icon(icon)
        ,_sex(sex){}

    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
};


//好友认证回复结构体
struct AuthRsp {
    AuthRsp(int peer_uid, QString peer_name, QString peer_nick,
            QString peer_icon, int peer_sex)
        :_uid(peer_uid)
        ,_name(peer_name)
        ,_nick(peer_nick)
        ,_icon(peer_icon)
        ,_sex(peer_sex)
    {}

    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
};

class TextChatData;
//好友信息结构体
struct FriendInfo {
    FriendInfo(int uid, QString name, QString nick,
               QString icon, int sex, QString desc,
               QString back, QString last_msg="")
        :_uid(uid)
        ,_name(name)
        ,_nick(nick)
        ,_icon(icon)
        ,_sex(sex)
        ,_desc(desc),
        _back(back)
        ,_last_msg(last_msg){}

    FriendInfo(std::shared_ptr<AuthInfo> auth_info)
        :_uid(auth_info->_uid)
        ,_nick(auth_info->_nick)
        ,_icon(auth_info->_icon)
        ,_name(auth_info->_name)
        ,_sex(auth_info->_sex){}

    FriendInfo(std::shared_ptr<AuthRsp> auth_rsp)
        :_uid(auth_rsp->_uid)
        ,_nick(auth_rsp->_nick)
        ,_icon(auth_rsp->_icon)
        ,_name(auth_rsp->_name)
        ,_sex(auth_rsp->_sex){}

    //添加聊天消息
    void AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData>> text_vec){
        for(const auto & text: text_vec){
            _chat_msgs.push_back(text);
        }
    }

    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    QString _desc;
    QString _back;
    QString _last_msg;
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};



//用户信息结构体
struct UserInfo{
    UserInfo(int uid, QString name, QString nick,
             QString icon, int sex, QString last_msg = "", QString desc="")
        :_uid(uid)
        ,_name(name)
        ,_nick(nick)
        ,_icon(icon)
        ,_sex(sex)
        ,_last_msg(last_msg)
        ,_desc(desc){}

    UserInfo(std::shared_ptr<AuthInfo> auth)
        :_uid(auth->_uid)
        ,_name(auth->_name)
        ,_nick(auth->_nick)
        ,_icon(auth->_icon)
        ,_sex(auth->_sex)
        ,_last_msg("")
        ,_desc(""){}

    UserInfo(std::shared_ptr<AuthRsp> auth)
        :_uid(auth->_uid)
        ,_name(auth->_name)
        ,_nick(auth->_nick)
        ,_icon(auth->_icon)
        ,_sex(auth->_sex)
        ,_last_msg(""){}

    UserInfo(std::shared_ptr<FriendInfo> friend_info)
        :_uid(friend_info->_uid)
        ,_name(friend_info->_name)
        ,_nick(friend_info->_nick)
        ,_icon(friend_info->_icon)
        ,_sex(friend_info->_sex),_last_msg(""){
        _chat_msgs = friend_info->_chat_msgs;
    }

    UserInfo(std::shared_ptr<SearchInfo> search_info)
        :_uid(search_info->_uid)
        ,_name(search_info->_name)
        ,_nick(search_info->_nick)
        ,_icon(search_info->_icon)
        ,_sex(search_info->_sex)
        ,_last_msg(""){}

    int _uid;//用户id
    QString _name;//用户名
    QString _nick;//昵称
    QString _icon;//头像
    int _sex;//性别
    QString _desc;//描述
    QString _last_msg;//最后一条消息
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};


//添加好友申请人
class AddFriendApply {
public:
    AddFriendApply(int from_uid, QString name, QString desc,
                   QString icon, QString nick, int sex)
        :_from_uid(from_uid)
        ,_name(name)
        ,_desc(desc)
        ,_icon(icon)
        ,_nick(nick)
        ,_sex(sex){}
    int _from_uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int     _sex;
};

//好友申请信息结构体
struct ApplyInfo{
    ApplyInfo(int uid, QString name, QString desc,
              QString icon, QString nick, int sex, int status)
        :_uid(uid),_name(name),_desc(desc),
        _icon(icon),_nick(nick),_sex(sex),_status(status){}

    ApplyInfo(std::shared_ptr<AddFriendApply> addinfo)
        :_uid(addinfo->_from_uid)
        ,_name(addinfo->_name)
        ,_desc(addinfo->_desc)
        ,_icon(addinfo->_icon)
        ,_nick(addinfo->_nick)
        ,_sex(addinfo->_sex)
        ,_status(0){}
    void SetIcon(QString head){
        _icon = head;
    }

    int _uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int _sex;
    int _status;
};

//文本聊天消息结构体
struct TextChatData{
    TextChatData(QString msg_id, QString msg_content, int fromuid, int touid)
        :_msg_id(msg_id)
        ,_msg_content(msg_content)
        ,_from_uid(fromuid)
        ,_to_uid(touid){}

    QString _msg_id;//消息id
    QString _msg_content;//消息内容
    int _from_uid;//发送者uid
    int _to_uid;//接收者uid
};

//文本聊天消息结构体
struct TextChatMsg{
    TextChatMsg(int fromuid, int touid, QJsonArray arrays)
        :_from_uid(fromuid)
        ,_to_uid(touid)
    {
        for(auto msg_data : arrays){
            auto msg_obj = msg_data.toObject();
            auto content = msg_obj["content"].toString();
            auto msgid = msg_obj["msgid"].toString();
            auto msg_ptr = std::make_shared<TextChatData>(msgid, content,fromuid, touid);
            _chat_msgs.push_back(msg_ptr);
        }
    }

    int _to_uid;
    int _from_uid;
    std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};


#endif // USERDATA_H
