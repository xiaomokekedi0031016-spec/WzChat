#pragma once
#include "const.h"

class HttpConnection :public std::enable_shared_from_this<HttpConnection>
{
public:
	HttpConnection(boost::asio::io_context& ioc);
	void Start();

private:
	tcp::socket _socket;
	beast::flat_buffer _buffer{ 8192 };
	http::request<http::dynamic_body> _request;
	http::response<http::dynamic_body> _response;

	net::steady_timer deadline_{
		_socket.get_executor(),std::chrono::seconds(60)
	};

	void CheckDeadline();
	void HandleReq();
	void WriteResponse();
};