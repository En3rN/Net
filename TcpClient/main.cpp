#include "TcpClient.h"
#include "logger.h"
#include <fstream>
#include "User.h"
#include "helpers.h"
#include "Packet.h"
#include "helpers.h"


class MyClient : public En3rN::Net::TcpClient
{
public:
	MyClient() {};
	MyClient(const char* aIp, int aPort, bool aConsoleth, bool aNetworkth, bool aLoop, int aTimeout)
	{
		settings.ip = aIp;	                // ip to accept from 
		settings.port = aPort;              // port to listen
		settings.consoleThread = aConsoleth;// console cin on a own thread
		settings.networkThread = aNetworkth;// networkthread looping in background -- accepting/ recving / sending
		settings.loop = aLoop;              // main thread loops within run()
		settings.timeout = aTimeout;        // timeout on wsapoll ms		
	}

};



int main() 
{
	using namespace En3rN::Net;
	MyClient client("gafi", 55000, true, true, false, 5);
	if (client.Init() == 0) client.Start();
	while (client.Update())
	{

		std::string s = "all " + Helpers::GenerateKey();
		Packet p(ServerPacket::Message);
		p << s;
		std::this_thread::sleep_for(std::chrono::seconds(1));		
		client.SendData(p);

	};	//if main tread not looping
	//std::cin.get();
	return 0;
}