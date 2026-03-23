#include "CServer.h"
#include "AsioIOServicePool.h"
#include "UserMgr.h"
#include "RedisMgr.h"
#include "ConfigMgr.h"

CServer::CServer(boost::asio::io_context& io_context, short port) 
	:_io_context(io_context), _port(port)
	,_acceptor(io_context, tcp::endpoint(tcp::v4(), port))
	, _timer(io_context, std::chrono::seconds(60))
{
	cout << "Server start success, listen on port : " << _port << endl;
	_timer.async_wait([this](boost::system::error_code e) {
		on_timer(e);
		});
	StartAccept();
}

CServer::~CServer(){
	cout << "Server destruct listen on port : " << _port << endl;
}

void CServer::HandleAccept(std::shared_ptr<CSession> new_session, const boost::system::error_code& error) {
	if (!error) {
		new_session->Start();
		std::lock_guard<std::mutex> lock(_mutex);
		//绑定sessionid和session
		_sessions.insert(std::make_pair(new_session->GetSessionId(), new_session));
	}
	else {
		cout << "session accept failed, error is " << error.what() << endl;
	}

	StartAccept();
}

void CServer::StartAccept() {
	auto& io_context = AsioIOServicePool::GetInstance()->GetIOService();
	std::shared_ptr<CSession> new_session = std::make_shared<CSession>(io_context, this);
	_acceptor.async_accept(new_session->GetSocket(), 
		std::bind(&CServer::HandleAccept, this, new_session, std::placeholders::_1)); 
}

void CServer::ClearSession(std::string session_id) {
	if (_sessions.find(session_id) != _sessions.end()) {
		auto uid = _sessions[session_id]->GetUserId();
		//移除用户uid和session的关联
		UserMgr::GetInstance()->RmvUserSession(uid, session_id);
	}
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_sessions.erase(session_id);
	}
}

void CServer::on_timer(const boost::system::error_code& e) {
	//lock_guard<std::mutex> lock(_mutex);
	//time_t now = time(nullptr);
	//for(auto iter = _sessions.begin(); iter != _sessions.end();) {
	//	auto b_expired = iter->second->IsHeartbeatExpired(now);
	//	if (b_expired) {
	//		iter->second->Close();
	//		iter = _sessions.erase(iter);
	//		auto uid_str = iter->second->GetUserId();
	//		auto lock_key = LOCK_PREFIX + uid_str;
	//		auto identifier = RedisMgr::GetInstance()->acquireLock(lock_key, LOCK_TIME_OUT, ACQUIRE_TIME_OUT);
	//		Defer defer([identifier, lock_key]() {
	//			RedisMgr::GetInstance()->releaseLock(lock_key, identifier);
	//			});
	//	}
	//	else {
	//		iter++;
	//	}
	//}
	std::vector<std::shared_ptr<CSession>> _expired_sessions;
	int session_count = 0;
	//此处加锁遍历session
	{
		lock_guard<mutex> lock(_mutex);
		time_t now = std::time(nullptr);
		for (auto iter = _sessions.begin(); iter != _sessions.end(); iter++) {
			auto b_expired = iter->second->IsHeartbeatExpired(now);
			if (b_expired) {
				//关闭socket, 其实这里也会触发async_read的错误处理
				iter->second->Close();
				//收集过期信息
				_expired_sessions.push_back(iter->second);
				continue;
			}
			session_count++;
		}
	}
	//设置session数量 todo..
	auto& cfg = ConfigMgr::Inst();
	auto self_name = cfg["SelfServer"]["Name"];
	auto count_str = std::to_string(session_count);
	RedisMgr::GetInstance()->HSet(LOGIN_COUNT, self_name, count_str);

	//处理过期session, 单独提出，防止死锁
	for (auto& session : _expired_sessions) {
		session->DealExceptionSession();
	}
	//再次设置，下一个60s检测
	_timer.expires_after(std::chrono::seconds(60));
	_timer.async_wait([this](boost::system::error_code ec) {
		on_timer(ec);
		});
}

bool CServer::CheckValid(std::string uuid)
{
	lock_guard<mutex> lock(_mutex);
	auto it = _sessions.find(uuid);
	if (it != _sessions.end()) {
		return true;
	}
	return false;
}