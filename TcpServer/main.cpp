#include "logger.h"
#include "helpers.h"
#include "TcpServer.h"
#include "Packet.h"
#include "tsQueue.h"
#include "Connection.h"

using namespace En3rN::Net;

enum class PacketType
{
	Message
};


class MyServer : public En3rN::Net::TcpServer
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
	virtual int onClientDisconnect() override
	{
		return 0;
	}
	virtual int ProcessPackets(tsQueue<Packet>& incManager, tsQueue<Packet>& outManager, const std::vector<std::shared_ptr<Connection>>& clients) override
	{
		TcpServer::ProcessPackets(incManager,outManager,clients);
		return 0;
	}

};

int main()
{
	MyServer server("0.0.0.0", 50000, true, true, true, 5);
	if (server.Init() == 0) server.Start();
	//while (server.Update()) {};	//if main tread not looping
	return 0;
}
