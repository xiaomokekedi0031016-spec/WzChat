#include "CSession.h"
#include "CServer.h"
#include <iostream>
#include <sstream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "LogicSystem.h"
#include "RedisMgr.h"
#include "ConfigMgr.h"

CSession::CSession(boost::asio::io_context& io_context, CServer* server) 
	:_socket(io_context), _server(server){
	boost::uuids::uuid a_uuid = boost::uuids::random_generator()();
	_session_id = boost::uuids::to_string(a_uuid);
	_recv_head_node = std::make_shared<MsgNode>(HEAD_TOTAL_LEN);
	std::cout << "[CSession] create new session, _session_id=" << _session_id << std::endl;
}

CSession::~CSession() {
	std::cout << "~CSession destruct" << endl;
}

tcp::socket& CSession::GetSocket() {
	return _socket;
}

std::string& CSession::GetSessionId() {
	return _session_id;
}

void CSession::SetUserId(int uid)
{
	_user_uid = uid;
}

int CSession::GetUserId()
{
	return _user_uid;
}

//逻辑和io涉及多线程访问
void CSession::Close() {
	std::lock_guard<std::mutex> lock(_session_mtx);
	std::cout << "[Close] connection closed (uuid=" << _session_id << ")" << std::endl;
	_socket.close();
	_b_close = true;
}

void CSession::Start() {
	std::cout << "[Start] begin reading head, _session_id=" << _session_id << std::endl;
	AsyncReadHead(HEAD_TOTAL_LEN);
}

//todo...
void CSession::AsyncReadHead(int total_len) {
	auto self = shared_from_this();
	asyncReadFull(HEAD_TOTAL_LEN, [this, self](const boost::system::error_code& ec, std::size_t bytes_transfered) {
		try {
			//if (ec) {
			//	std::cout << "handle read failed, error is " << ec.what() << endl;
			//	Close();
			//	_server->ClearSession(_session_id);
			//	return;
			//}
			if (ec) {
				std::cout << "handle read failed, error is " << ec.what() << endl;
				Close();
				DealExceptionSession();
				return;
			}
			if (bytes_transfered < HEAD_TOTAL_LEN) {
				std::cout << "read length not match, read [" << bytes_transfered << "] , total ["
					<< HEAD_TOTAL_LEN << "]" << endl;
				Close();
				_server->ClearSession(_session_id);
				return;
			}

			if (!_server->CheckValid(_session_id)) {
				Close();
				return;
			}

			_recv_head_node->Clear();
			memcpy(_recv_head_node->_data, _data, bytes_transfered);
			//id
			short msg_id = 0;
			memcpy(&msg_id, _recv_head_node->_data, HEAD_ID_LEN);
			msg_id = boost::asio::detail::socket_ops::network_to_host_short(msg_id);
			std::cout << "msg_id is " << msg_id << endl;
			if (msg_id > MAX_LENGTH) {
				std::cout << "invalid msg_id is " << msg_id << endl;
				Close();
				_server->ClearSession(_session_id);
				return;
			}
			//len
			short msg_len = 0;
			memcpy(&msg_len, _recv_head_node->_data + HEAD_ID_LEN, HEAD_DATA_LEN);
			msg_len = boost::asio::detail::socket_ops::network_to_host_short(msg_len);
			std::cout << "msg_len is " << msg_len << endl;
			if (msg_len > MAX_LENGTH) {
				std::cout << "invalid data length is " << msg_len << endl;
				_server->ClearSession(_session_id);
				return;
			}
			_recv_msg_node = make_shared<RecvNode>(msg_len, msg_id);
			AsyncReadBody(msg_len);
			//更新session心跳时间
			UpdateHeartbeat();

		}
		catch (std::exception&e) {
			std::cout << "Exception code is " << e.what() << endl;
		}
	});
}

void CSession::asyncReadFull(std::size_t maxLength, std::function<void(const boost::system::error_code&, std::size_t)> handler) {
	::memset(_data, 0, sizeof(_data));
	asyncReadLen(0, maxLength, handler);
}

void CSession::asyncReadLen(std::size_t read_len, std::size_t total_len, std::function<void(const boost::system::error_code&, std::size_t)> handler) {
	auto self = shared_from_this();
	//写入到_data缓冲区中，从偏移read_len开始，最多读total_len - read_len字节
	_socket.async_read_some(boost::asio::buffer(_data + read_len, total_len - read_len), [read_len, handler, total_len, self](const boost::system::error_code& ec, std::size_t bytesTransfered) {
		if (ec) {
			handler(ec, read_len);
			return;
		}
		if (read_len + bytesTransfered >= total_len) {
			handler(ec, read_len + bytesTransfered);
			return;
		}
		else {
			//继续读取剩余数据
			self->asyncReadLen(read_len + bytesTransfered, total_len, handler);
		}
	});
}

void CSession::AsyncReadBody(int total_len) {
	auto self = shared_from_this();
	asyncReadFull(total_len, [this, self, total_len](const boost::system::error_code& ec, std::size_t bytes_transfered) {
		try {
			//qt客户端主动关闭连接后触发或者网络异常导致连接断开后触发
			if (ec) {
				std::cout << "handle read failed, error is " << ec.what() << endl;
				Close();
				DealExceptionSession();
				return;
			}
			//if (ec) {
			//	std::cout << "handle read failed, error is " << ec.what() << endl;
			//	Close();
			//	//加锁清除session
			//	//如果 Redis 中的 session id 和当前 session 不一致，说明用户已经在另一台服务器登录了（异地登录），这个 session 不需要删除 Redis 数据，直接 return 即可。
			//	//如果一致，说明是这个 session 异常掉线，可以安全地删除 Redis 中的登录状态
			//	auto uid_str = std::to_string(_user_uid);
			//	auto lock_key = LOCK_PREFIX + uid_str;
			//	auto identifier = RedisMgr::GetInstance()->acquireLock(lock_key, LOCK_TIME_OUT, ACQUIRE_TIME_OUT);
			//	Defer defer([identifier, lock_key, self, this]() {
			//		_server->ClearSession(_session_id);
			//		RedisMgr::GetInstance()->releaseLock(lock_key, identifier);
			//		});
			//	if (identifier.empty()) {
			//		return;
			//	}
			//	std::string redis_session_id = "";
			//	auto bsuccess = RedisMgr::GetInstance()->Get(USER_SESSION_PREFIX + uid_str, redis_session_id);
			//	if (!bsuccess) {
			//		return;
			//	}
			//	if (redis_session_id != _session_id) {
			//		//说明有客户在其他服务器异地登录了
			//		return;
			//	}
			//	RedisMgr::GetInstance()->Del(USER_SESSION_PREFIX + uid_str);
			//	//清除用户登录信息
			//	RedisMgr::GetInstance()->Del(USERIPPREFIX + uid_str);
			//	return;
			//}

			if(bytes_transfered < total_len){
				std::cout << "read length not match, read [" << bytes_transfered << "] , total ["
					<< total_len << "]" << endl;
				Close();
				return;
			}
			//处理的是t了后，被t的还没有完全退出，退出之前发了消息然后才被t的
			//判断连接无效
			if (!_server->CheckValid(_session_id)) {
				Close();
				return;
			}

			memcpy(_recv_msg_node->_data, _data, bytes_transfered);
			_recv_msg_node->_cur_len += bytes_transfered;
			_recv_msg_node->_data[_recv_msg_node->_total_len] = '\0';
			cout << "receive data is " << _recv_msg_node->_data << endl;



			LogicSystem::GetInstance()->PostMsgToQue(make_shared<LogicNode>(shared_from_this(), _recv_msg_node));
			AsyncReadHead(HEAD_TOTAL_LEN);

			//更新session心跳时间
			UpdateHeartbeat();
		}
		catch(std::exception& e) {
			std::cout << "Exception code is " << e.what() << endl;
		}
	});
}

void CSession::Send(char* msg, short max_length, short msgid) {
	std::lock_guard<std::mutex> lock(_send_lock);
	int send_que_size = _send_que.size();
	if (send_que_size > MAX_SENDQUE) {
		std::cout << "session: " << _session_id << " send que fulled, size is " << MAX_SENDQUE << endl;
		return;
	}

	_send_que.push(make_shared<SendNode>(msg, max_length, msgid));

	if (send_que_size > 0) {
		return;
	}
	auto& msgnode = _send_que.front();
	boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
		std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
}

void CSession::Send(std::string msg, short msgid) {
	std::lock_guard<std::mutex> lock(_send_lock);
	int send_que_size = _send_que.size();
	if (send_que_size > MAX_SENDQUE) {
		std::cout << "session: " << _session_id << " send que fulled, size is " << MAX_SENDQUE << endl;
		return;
	}
	_send_que.push(make_shared<SendNode>(msg.c_str(), msg.length(), msgid));
	if (send_que_size > 0) {//保证同一时间只有一个异步发送在进行
		return;
	}
	auto& msgnode = _send_que.front();
	std::cout << "[Send] start async_write, len=" << msgnode->_total_len << " uuid=" << _session_id << std::endl;
	boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
		std::bind(&CSession::HandleWrite, this, std::placeholders::_1, SharedSelf()));
}

void CSession::HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self) {
	try {
		auto self = shared_from_this();
		if (!error) {
			std::lock_guard<std::mutex> lock(_send_lock);
			//cout << "send data " << _send_que.front()->_data+HEAD_LENGTH << endl;
			_send_que.pop();
			if (!_send_que.empty()) {
				auto& msgnode = _send_que.front();
				boost::asio::async_write(_socket, boost::asio::buffer(msgnode->_data, msgnode->_total_len),
					std::bind(&CSession::HandleWrite, this, std::placeholders::_1, shared_self));
			}
		}
		else {
			std::cout << "handle write failed, error is " << error.what() << endl;
			Close();
			DealExceptionSession();
		}
	}
	catch (std::exception& e) {
		std::cout << "Exception code: " << e.what() << endl;
	}
}

LogicNode::LogicNode(shared_ptr<CSession> session,
	shared_ptr<RecvNode> recvnode) :_session(session), _recvnode(recvnode) {

}

std::shared_ptr<CSession> CSession::SharedSelf() {
	return shared_from_this();
}

void CSession::NotifyOffline(int uid) {

	Json::Value rtvalue;
	rtvalue["error"] = ErrorCodes::Success;
	rtvalue["uid"] = uid;

	std::string return_str = rtvalue.toStyledString();

	Send(return_str, ID_NOTIFY_OFF_LINE_REQ);
	return;
}

bool CSession::IsHeartbeatExpired(std::time_t& now) {
	double diff_sec = std::difftime(now, _last_heartbeat);
	if (diff_sec > 20) {
		std::cout << "heartbeat expired, session id is  " << _session_id << endl;
		return true;
	}

	return false;
}
void CSession::UpdateHeartbeat()
{
	time_t now = std::time(nullptr);
	_last_heartbeat = now;
}

void CSession::DealExceptionSession() {
	auto self = shared_from_this();
	//加锁清除session
	auto uid_str = std::to_string(_user_uid);
	auto lock_key = LOCK_PREFIX + uid_str;
	//分布式锁
	auto identifier = RedisMgr::GetInstance()->acquireLock(lock_key, LOCK_TIME_OUT, ACQUIRE_TIME_OUT);
	Defer defer([identifier, lock_key, self, this]() {
		_server->ClearSession(_session_id);
		RedisMgr::GetInstance()->releaseLock(lock_key, identifier);
		});
	if (identifier.empty()) {
		return;
	}
	std::string redis_session_id = "";
	auto bsuccess = RedisMgr::GetInstance()->Get(USER_SESSION_PREFIX + uid_str, redis_session_id);
	if (!bsuccess) {
		return;
	}

	if (redis_session_id != _session_id) {
		//说明有客户在其他服务器异地登录了
		return;
	}

	RedisMgr::GetInstance()->Del(USER_SESSION_PREFIX + uid_str);
	//清除用户登录信息
	RedisMgr::GetInstance()->Del(USERIPPREFIX + uid_str);
}