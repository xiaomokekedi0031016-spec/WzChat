#pragma once
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include <mutex>

using message::StatusService;
using message::GetChatServerReq;
using message::GetChatServerRsp;
using grpc::ServerContext;
using grpc::Status;

using message::LoginReq;
using message::LoginRsp;

class ChatServer {
public:
	std::string host;
	std::string port;
	std::string name;
	int con_count;
};

class StatusServiceImpl final : public StatusService::Service {
public:
	StatusServiceImpl();
	Status GetChatServer(ServerContext* context, const GetChatServerReq* request,
		GetChatServerRsp* reply) override;
	Status Login(ServerContext* context, const LoginReq* request,
		LoginRsp* reply) override;

private:
	ChatServer getChatServer();
	void insertToken(int uid, std::string token);
	std::unordered_map<std::string, ChatServer> _servers;
	std::mutex _server_mtx;
};