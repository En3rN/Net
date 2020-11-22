#pragma once
#include "AppSettings.h"
#include "Connection.h"
#include <vector>
#include "User.h"
#include "Packet.h"
#include "tsQueue.h"

namespace En3rN
{
	namespace Net
	{		
		class TcpServer
		{
		protected:			
			AppSettings settings;			
			std::vector<std::shared_ptr<Connection>>	connections{};
			std::vector<WSAPOLLFD>						pollFDS;
			bool										m_running = false;
			tsQueue<Packet>								incManager;
			tsQueue<Packet>								outManager;

			int NetworkFrame();
			int Console();
			int CloseConnection(int index, const std::string& reason);
			int SendToAll(Packet& packet, std::shared_ptr<Connection> ignoreClient = nullptr);
			int GetConnection(uint16_t aID);
			int GetConnection(std::string& aUserName);
			
		public:
			TcpServer();			
			virtual ~TcpServer();			
			virtual int onClientConnect(std::shared_ptr<Connection> newClient);
			virtual int onClientDisconnect();
			virtual int ProcessPackets(tsQueue<Packet>& incManager, tsQueue<Packet>& outManager, const std::vector<std::shared_ptr<Connection>>& clients);	
			bool		Update();
			int SendData(Packet& packet);
			int Init();
			int Start();
			int Stop();

		};
	}
}
