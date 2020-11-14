#include "TcpClient.h"
#include "logger.h"
#include <fstream>
#include "User.h"
#include "helpers.h"
#include "Packet.h"

class MyClient : public En3rN::Net::TcpClient
{
	 

};



int main() 
{
	
	MyClient client;
	if (client.Init() == 0)	
		client.Start();
	
	//std::cin.get();
	return 0;
}