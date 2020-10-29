
#include "TcpServer.h"
#include "logger.h"

class HDServer: public TcpServer
{
public:
	HDServer(const char* ipAddress, int port, bool console = false)
	{
		m_ipAddress = ipAddress;
		m_port = port;
		m_console = console;
		ConnectedUsers.emplace_back("console", "", lvlAdmin);
	}
private:
};


int main()
{	
	logger::LogLevel = Info;
	HDServer server("0.0.0.0",54000,true);
	if (server.initServer() != 0)
	{
		logger(Error) << "Could not init Server " << WSAGetLastError;
		return WSAGetLastError();
	}
	else
		logger(Info) << "Starting Server";
		server.run();

	return 0;
}

