#include "LogicSystem.h"
#include "HttpConnection.h"

LogicSystem::LogicSystem() {
	RegGet("/get_test", [](std::shared_ptr<HttpConnection> connection) {
		//beast::ostream 可以像标准输出一样写入到响应体
		beast::ostream(connection->_response.body()) << "receive get_test req";
		int i = 0;
		for (auto& elem : connection->_get_params) {
			i++;
			beast::ostream(connection->_response.body()) << "param" << i << " key is " << elem.first;
			beast::ostream(connection->_response.body()) << ", " << " value is " << elem.second << std::endl;
		}
		});
}	

void LogicSystem::RegGet(std::string path, HttpHandler handler) {
	_get_handlers.insert(std::make_pair(path, handler));
}

void LogicSystem::RegPost(std::string path, HttpHandler handler) {
	_post_handlers.insert(std::make_pair(path, handler));
}

bool LogicSystem::HandleGet(std::string path, std::shared_ptr<HttpConnection> con) {
	if(_get_handlers.find(path) == _get_handlers.end()){
		return false;
	}
	_get_handlers[path](con);
	return true;
}

bool LogicSystem::HandlePost(std::string path, std::shared_ptr<HttpConnection> con) {
	if (_post_handlers.find(path) == _post_handlers.end()) {
		return false;
	}
	_post_handlers[path](con);
	return true;
}

LogicSystem::~LogicSystem() {
	
}