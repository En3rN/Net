#include "logger.h"
#include "helpers.h"
#include "TcpServer.h"
#include "Packet.h"
#include "tsQue.h"
#include "Connection.h"
#include <thread>
#include "tsVec.h"

using namespace En3rN::Net;

class MyServer : public TcpServer
{
public:	

	MyServer() {};
	MyServer(const char* aIp, int aPort, bool aConsoleth, bool aNetworkth, bool aLoop, int aTimeout)
	{
		settings.ip = aIp;	                // ip to accept from 
		settings.port = aPort;              // port to listen
		settings.consoleThread = aConsoleth;// console cin on a own thread
		settings.networkThread = aNetworkth;// networkthread looping in background -- accepting/ recving / sending
		settings.loop = aLoop;              // main thread loops within run()
		settings.timeout = aTimeout;        // timeout on wsapoll ms		
	}

	virtual int onClientConnect(std::shared_ptr<Connection> client) override
	{
		TcpServer::onClientConnect(client); // sends out welcome msg;
		return 0;
	}
	virtual int onClientDisconnect(std::shared_ptr<Connection> client) override
	{
		return 0;
	}
	virtual int onMessage(Packet& packet) override
	{
		switch (packet.header.type)
		{
			// add packet types in packettype.h
		default:
			logger(LogLvl::Warning) << "Unknown packet type!";
			break;
		}
		return 0;
	}

};

int main()
{
	MyServer server("0.0.0.0", 50000, true, true, true, 5);	

	if (server.Init() == 0) server.Start();
	while (server.Update()) 
	{
		std::string s = "all " +Helpers::GenerateKey();
		Packet p;
		p << s;
		std::this_thread::sleep_for(std::chrono::seconds(1));
		server.SendData(p);
	
	};	//if main tread not looping
	return 0;
}
