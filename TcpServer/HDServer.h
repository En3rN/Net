//#pragma once
//#include "TcpServer.h"
//#include "User.h"
//#include "Commands.h"
//#include "Packet.h"
//
//using namespace En3rN::Net;
//
//class HDServer : public TcpServer
//{	
//	//std::vector<User> ConnectedUsers = { User("console","", UserLvl::Admin) };
//	
//public:
//	HDServer(int port, bool console = false);		
//	~HDServer();
//	void Running(bool running);
//	int runCommand(User& sender, std::vector<std::string>& parsed);
//	
//private:
//	bool OnUserCreate() override;
//	bool OnUserUpdate(tsQueue<Packet>& incManager, tsQueue<Packet>& outManager, const std::vector<User>& ConnectedUsers) override;
//};
