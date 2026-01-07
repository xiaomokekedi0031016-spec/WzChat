#include <iostream>
#include "ConfigMgr.h"
#include "const.h"
#include "CServer.h"

int main()
{
	auto& config = ConfigMgr::Inst();
	std::string gate_server_port = config["GateServer"]["Port"];
	unsigned short port = atoi(gate_server_port.c_str());
	std::cout << "Gate Server Port: " << port << std::endl;

	try {
		net::io_context ioc{ 1 };
		net::signal_set signals(ioc, SIGINT, SIGTERM);
		signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number) {
			if (error) {
				return;
			}
			ioc.stop();
		});
		//todo...
#if 0 
		std::shared_ptr<CServer> server(new CServer(ioc, port));
		server->Start();
#endif
		std::make_shared<CServer>(ioc, port)->Start();
		ioc.run();
	}
	catch (std::exception const& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	

	return 0;
}

