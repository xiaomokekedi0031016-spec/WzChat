#pragma once
#include "const.h"
#include <grpcpp/grpcpp.h>
#include "message.grpc.pb.h"
#include "ConfigMgr.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class RPConPool {
public:
	RPConPool(size_t poolsize, std::string host, std::string port) 
		: poolSize_(poolsize)
		, host_(host)
		, port_(port){
		for (size_t i = 0; i < poolSize_; ++i) {
			std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port, grpc::InsecureChannelCredentials());
			connections_.push(VarifyService::NewStub(channel));
		}
	}
	
	~RPConPool() {
		Close();
		std::lock_guard<std::mutex> lock(mutex_);
		while (!connections_.empty()) {
			connections_.pop();
		}
		
	}

	std::unique_ptr<VarifyService::Stub> getConnection() {
		std::unique_lock<std::mutex> lock(mutex_);
		cond_.wait(lock, [this]() {
			if (b_stop_) {
				return true;
			}
			return !connections_.empty();
		});
		if(b_stop_) {
			return nullptr;
		}
		auto context = std::move(connections_.front());
		connections_.pop();
		return context;
	}

	void returnConnection(std::unique_ptr<VarifyService::Stub> context) {
		std::lock_guard<std::mutex> lock(mutex_);
		if (b_stop_) {
			return;
		}
		connections_.push(std::move(context));
		cond_.notify_one();
		return;
	}
	
	void Close() {
		std::lock_guard<std::mutex> lock(mutex_);
		b_stop_ = true;
		cond_.notify_all();
	}

private:
	std::atomic<bool> b_stop_ = false;
	size_t poolSize_;
	std::mutex mutex_;	
	std::condition_variable cond_;
	std::queue<std::unique_ptr<VarifyService::Stub>> connections_;
	std::string	host_;
	std::string	port_;
};

class VerifyGrpcClient : public Singleton<VerifyGrpcClient> {
	friend class Singleton<VerifyGrpcClient>;
public:
	GetVarifyRsp GetVarifyCode(std::string email) {
		ClientContext context;
		GetVarifyReq request;
		GetVarifyRsp reply;
		request.set_email(email);
		auto stub = pool_->getConnection();
		Status status = stub->GetVarifyCode(&context, request, &reply);
		if(status.ok()) {
			pool_->returnConnection(std::move(stub));
			return reply;
		} else {
			pool_->returnConnection(std::move(stub));
			GetVarifyRsp errorReply;
			errorReply.set_error(ErrorCodes::RPCFailed);
			return errorReply;
		}
	}

private:
	VerifyGrpcClient();
	std::unique_ptr<RPConPool> pool_;
};