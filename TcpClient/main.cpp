#include "TcpClient.h"
#include "logger.h"
#include <fstream>
#include "User.h"
#include "helpers.h"
#include "Packet.h"


//class Client : public TcpClient
//{
//	User user;
//
//public:
//	Client(const char* ipAddress, int port)
//	{
//		m_ipAddress = ipAddress;
//		m_port = port;
//	}
//	bool onUserCreate() override
//	{
//		std::fstream saveduser("user.dat");
//		
//		saveduser >> user;
//		saveduser.close();
//		if (user.Name != "noname")
//		{
//			std::string cmd = "auth " + user.Name + " " + user.Key+ "n/a";
//			if (onSend(cmd) != 0)
//				logger(LogLvl::Error) << "Error sending msg! " << WSAGetLastError;
//			logger(LogLvl::Debug) << "Sendt: " << cmd;
//		}
//		return true;
//	}
//	bool onUserUpdate() override
//	{		
//		std::string cmd;
//
//		if(GetKeyState(VK_ESCAPE) == true)
//		{
//			std::getline(std::cin, cmd);
//			if (cmd == "exit")
//				return false;
//			else
//				if (onSend(cmd) != 0)
//					logger(LogLvl::Error) << "Error sending msg! " << WSAGetLastError;			
//		}
//		return true;
//	}
//	int onSend(std::string& msg) override
//	{
//		
//		if (msg == "") return 0;
//		std::vector<std::string> cmdVec = Split(msg, ' ');
//		if (cmdVec[0] == "auth" && cmdVec.size() > 1)
//		{
//			user.Name = cmdVec[1];
//			std::fstream saveduser("user.dat", std::ios::out);
//			saveduser << user;
//			saveduser.close();
//		}
//		//create and send packet
//		Packet outPacket(clSocket);
//		outPacket << msg;
//		uint16_t bytesSendt = 0;
//		uint16_t packetSize = outPacket.Size();
//		uint16_t remainingSize = packetSize - bytesSendt;
//		
//		while (bytesSendt < packetSize)
//		{
//			bytesSendt += send(clSocket, &outPacket.buffer[bytesSendt], remainingSize, 0);
//			logger(LogLvl::Debug) << bytesSendt;
//			remainingSize -= bytesSendt;
//		}
//		if (bytesSendt == SOCKET_ERROR)
//		{
//			logger(LogLvl::Error) << "Error sending data: " << WSAGetLastError();
//			return WSAGetLastError();
//		}
//		else			
//			logger(LogLvl::Debug) << bytesSendt << "/" << packetSize << " bytes sendt!";
//
//		return 0;
//	}
//	int onReceive(std::string& msg) override
//	{		
//		std::vector<std::string> cmdVec = Split(msg, ' ');	
//		
//		if (cmdVec[0] == "Key")
//		{
//			user.Key = cmdVec[2];
//			std::fstream saveduser("user.dat", std::ios::out);
//			saveduser << user;
//			saveduser.close();
//
//		}
//		logger(LogLvl::Debug) << msg;
//		return 0;
//
//	}
//};




int main() 
{
	using namespace En3rN::Net;
	TcpClient client;
	if (client.Init() == 0)	
		client.Start();
	
	std::cin.get();
	return 0;
}