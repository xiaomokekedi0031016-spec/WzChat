#include "HttpConnection.h"

HttpConnection::HttpConnection(boost::asio::io_context& ioc) 
	:_socket(ioc){

}

void HttpConnection::Start() {
	auto self = shared_from_this();
	http::async_read(_socket, _buffer, _request, [self](beast::error_code ec, std::size_t bytes_transferred) {
		try {
			if (ec) {
				std::cout << "http read err is " << ec.what() << std::endl;
				return;
			}
			boost::ignore_unused(bytes_transferred);
			self->HandleReq();

		}catch (std::exception& exp) {
			std::cout << "exception is " << exp.what() << std::endl;
		}


	});
}

void HttpConnection::HandleReq() {
	_response.version(_request.version());
	_response.keep_alive(false);
	if (_request.method() == http::verb::get) {
		//tode...

	}
	if(_request.method() == http::verb::post){
		//tode...
	}

}