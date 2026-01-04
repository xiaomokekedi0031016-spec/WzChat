#pragma once
#include "const.h"
#include <map>
#include <functional>

class HttpConnection;
using HttpHandler = std::function<void(std::shared_ptr<HttpConnection>)>;

class LogicSystem : public Singleton<LogicSystem>
{
	friend class Singleton<LogicSystem>;
	
public:
	~LogicSystem();
	void RegGet(std::string path, HttpHandler handler);
	void RegPost(std::string path, HttpHandler handler);
	bool HandleGet(std::string path, std::shared_ptr<HttpConnection> con);
	bool HandlePost(std::string path, std::shared_ptr<HttpConnection> con);

private:
	LogicSystem();
	std::map<std::string, HttpHandler> _get_handlers;
	std::map<std::string, HttpHandler> _post_handlers;
};