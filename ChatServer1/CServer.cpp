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
	//为什么要把share_from_this()放在构造函数外面调用?
	//在构造函数内部调用shared_from_this()会导致std::bad_weak_ptr异常，因为对象还没有完全构造完成，shared_ptr还没有被创建。
	cout << "Server start success, listen on port : " << _port << endl;
	StartAccept();
}

//为什么析构函数中的_timer.cancel()要放在析构函数外面调用?
//析构函数中的_timer.cancel()放在析构函数外面调用是为了避免在对象已经开始销毁时调用cancel()，这可能会导致访问已经被销毁的对象，从而引发未定义行为。通过在析构函数外部调用StopTimer()，可以确保在对象完全销毁之前取消定时器，避免潜在的访问野指针问题。
//一句话：在自己销毁的过程中，触发了访问自己的回调

CServer::~CServer(){
	cout << "Server destruct listen on port : " << _port << endl;
	//_timer.cancel();//cancel()只能取消尚未完成的异步操作，并会以operation_aborted触发一次回调；但对于已经开始或即将执行的handler，无法中断。
	//尚未完成的异步操作：已经注册了，但 handler 还没有被调用 >> cancel() 并不是立即停止执行 handler，它只是标记操作被取消，io_context 会在合适的时机调用 handler。
	//即将执行状态有点模糊，它通常是指操作已经在队列中，但线程调度很快就会调用 handler。
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
	if (e) {
		std::cout << "timer error: " << e.message() << std::endl;
		return;
	}
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

void CServer::StartTimer()
{
	//为什么这里使用shared_from_this()?
	// 执行顺序有可能是：调用CServer析构函数->调用timer的cancel函数->触发timer回调函数on_timer->on_timer中访问this指针，导致访问野指针崩溃
	//启动定时器
	auto self(shared_from_this());
	_timer.async_wait([self](boost::system::error_code ec) {
		self->on_timer(ec);
		});
}

void CServer::StopTimer()
{
	_timer.cancel();
}
