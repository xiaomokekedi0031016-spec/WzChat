#pragma once
#include <boost/asio.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <queue>
#include <mutex>
#include <memory>
#include "MsgNode.h"
#include "const.h"

using namespace std;
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class CServer;
class CSession : public std::enable_shared_from_this<CSession> {
public:
	CSession(boost::asio::io_context& io_context, CServer* server);
	~CSession();
	tcp::socket& GetSocket();
	std::string& GetSessionId();
	void Start();
	void SetUserId(int uid);
	int GetUserId();
	void Close();//关闭连接
	void AsyncReadHead(int total_len);//开始读头部
	void AsyncReadBody(int total_len);//开始读消息体
	//发送消息
	void Send(char* msg, short max_length, short msgid);
	void Send(std::string msg, short msgid);
	std::shared_ptr<CSession> SharedSelf();

private:
	void asyncReadFull(std::size_t maxLength, 
		std::function<void(const boost::system::error_code&, std::size_t)> handler);//读满指定长度的数据
	void asyncReadLen(std::size_t read_len, std::size_t total_len,
		std::function<void(const boost::system::error_code&, std::size_t)> handler);//读指定长度的数据
	void HandleWrite(const boost::system::error_code& error, std::shared_ptr<CSession> shared_self);//发送完成回调

private:
	tcp::socket _socket;
	std::string _session_id;//session唯一标识	
	int _user_uid;//用户id
	CServer* _server;
	std::shared_ptr<MsgNode> _recv_head_node;//收到的头部结构	
	char _data[MAX_LENGTH];
	std::mutex _session_mtx;//session锁
	bool _b_close;
	std::shared_ptr<RecvNode> _recv_msg_node;//收到的消息结构
	std::mutex _send_lock;//发送锁
	std::queue<shared_ptr<SendNode>> _send_que;//发送队列

};

//逻辑节点
class LogicNode {
	friend class LogicSystem;
public:
	LogicNode(shared_ptr<CSession>, shared_ptr<RecvNode>);
private:
	shared_ptr<CSession> _session;
	shared_ptr<RecvNode> _recvnode;
};
