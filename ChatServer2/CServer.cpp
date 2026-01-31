#include "CServer.h"
#include "AsioIOServicePool.h"
#include "UserMgr.h"

CServer::CServer(boost::asio::io_context& io_context, short port) 
	:_io_context(io_context), _port(port),
	_acceptor(io_context, tcp::endpoint(tcp::v4(), port))
{
	cout << "Server start success, listen on port : " << _port << endl;
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