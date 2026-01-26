#ifndef USERDATA_H
#define USERDATA_H
#include <QString>

//用户信息结构体
struct UserInfo{
    UserInfo(int uid, QString name, QString nick, QString icon, int sex, QString last_msg = "", QString desc="")
        :_uid(uid)
        ,_name(name)
        ,_nick(nick)
        ,_icon(icon)
        ,_sex(sex)
        ,_last_msg(last_msg)
        ,_desc(desc){}


    int _uid;//用户id
    QString _name;//用户名
    QString _nick;//昵称
    QString _icon;//头像
    int _sex;//性别
    QString _desc;//描述
    QString _last_msg;//最后一条消息
};

//搜索信息结构体
class SearchInfo {
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex, QString icon)
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

//添加好友申请
class AddFriendApply {
public:
    AddFriendApply(int from_uid, QString name, QString desc,
                   QString icon, QString nick, int sex);
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

    int _uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int _sex;
    int _status;
};

struct FriendInfo {

};

struct TextChatData{

};

#endif // USERDATA_H
