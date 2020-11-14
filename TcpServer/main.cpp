#include "logger.h"
#include "helpers.h"
#include "TcpServer.h"
#include "Packet.h"
#include "tsQueue.h"
#include "Connection.h"

using namespace En3rN::Net;

class MyServer : public En3rN::Net::TcpServer	
{
public:

	virtual int onClientConnect() override
	{
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
	MyServer server;	
	server.Start();	
	return 0;
}
