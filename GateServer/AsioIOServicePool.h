#pragma once
#include "Singleton.h"
#include <boost/asio.hpp>

class AsioIOServicePool :public Singleton<AsioIOServicePool>
{
	friend Singleton<AsioIOServicePool>;
public:
	using IOService = boost::asio::io_context;
	using Work = boost::asio::io_context::work;
	using WorkPtr = std::unique_ptr<Work>;

	~AsioIOServicePool();
	AsioIOServicePool(const AsioIOServicePool&) = delete;
	AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;

	boost::asio::io_context& GetIOService();
	void Stop();

private:
	AsioIOServicePool(std::size_t size = std::thread::hardware_concurrency());
	
private:
	std::vector<IOService> _ioServices;
	std::vector<WorkPtr> _works;
	std::vector<std::thread> _threads;
	std::atomic_size_t _nextIOService;
};