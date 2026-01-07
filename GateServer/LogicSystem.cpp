#include "LogicSystem.h"
#include "HttpConnection.h"
#include "VerifyGrpcClient.h"
#include "RedisMgr.h"
#include "MysqlMgr.h"

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

	RegPost("/get_varifycode", [](std::shared_ptr<HttpConnection>connection) {
		auto body = boost::beast::buffers_to_string(connection->_request.body().data());
		//std::cout << "receive body is " << body << std::endl;
		connection->_response.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(body, src_root);
		if (!parse_success) {
			std::cout << "Failed to parse JSON data!" << std::endl;
			root["error"] = ErrorCodes::Error_Json;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return;
		}
		if(!src_root.isMember("email")){
			std::cout << " no email field" << std::endl;
			root["error"] = ErrorCodes::Error_Json;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return ;
		}
		auto email = src_root["email"].asString();
		std::cout << "email is " << email << std::endl;
		//varifyServer的rsp回复
		GetVarifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVarifyCode(email);
		root["error"] = rsp.error();
		root["email"] = email;
		std::string jsonstr = root.toStyledString();
		beast::ostream(connection->_response.body()) << jsonstr;
		return;
	});
	
	RegPost("/user_register",[](std::shared_ptr<HttpConnection>connection) {
		auto body = boost::beast::buffers_to_string(connection->_request.body().data());	
		connection->_response.set(http::field::content_type, "text/json");
		Json::Value root;
		Json::Reader reader;
		Json::Value src_root;
		bool parse_success = reader.parse(body, src_root);
		if (!parse_success) {
			std::cout << "Failed to parse JSON data!" << std::endl;
			root["error"] = ErrorCodes::Error_Json;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return;
		}
		auto email = src_root["email"].asString();
		auto name = src_root["user"].asString();
		auto pwd = src_root["passwd"].asString();
		auto confirm = src_root["confirm"].asString();
		auto icon = src_root["icon"].asString();
		std::string varify_code;
		bool b_get_varify = RedisMgr::GetInstance()->Get(CODEPREFIX + email, varify_code);
		if (!b_get_varify) {
			std::cout << " get varify code expired" << std::endl;
			root["error"] = ErrorCodes::VarifyExpired;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return;
		}
		if (varify_code != src_root["varifycode"].asString()) {
			std::cout << " varify code error" << std::endl;
			root["error"] = ErrorCodes::VarifyCodeErr;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return;
		}
		//查找Mysql数据库判断用户是否存在(取出uid)
		int uid = MysqlMgr::GetInstance()->RegUser(name, email, pwd, icon);
		if (uid == 0 || uid == -1) {
			std::cout << " user or email exist" << std::endl;
			root["error"] = ErrorCodes::UserExist;
			std::string jsonstr = root.toStyledString();
			beast::ostream(connection->_response.body()) << jsonstr;
			return;
		}
		root["error"] = 0;
		root["email"] = email;
		root["uid"] = uid;
		root["user"] = name;
		root["passwd"] = pwd;
		root["icon"] = icon;
		root["confirm"] = confirm;
		root["varifycode"] = src_root["varifycode"].asString();
		std::string jsonstr = root.toStyledString();
		beast::ostream(connection->_response.body()) << jsonstr;
		return;
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