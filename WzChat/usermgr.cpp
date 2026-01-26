#include "usermgr.h"




UserMgr::~UserMgr()
{

}



UserMgr::UserMgr()
    :_user_info(nullptr)
{

}



void UserMgr::SetToken(QString token)
{
    _token = token;
}



void UserMgr::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
}



int UserMgr::GetUid()
{
    return _user_info->_uid;
}

