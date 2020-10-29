#include "logger.h"
#include "helpers.h"
#include "Commands.h"
#include "TcpServer.h"
#include "Packet.h"



int main()
{	
	using namespace En3rN::Net;
	TcpServer server = TcpServer();
	if(server.Init()==0)
		server.Start();
	
	
	return 0;
}
