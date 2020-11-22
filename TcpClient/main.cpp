#include "TcpClient.h"
#include "logger.h"
#include <fstream>
#include "User.h"
#include "helpers.h"
#include "Packet.h"

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
	MyClient client; //default construction (localhost,5000,true,true,true,5)
	if (client.Init() == 0) client.Start();
	return 0;
}