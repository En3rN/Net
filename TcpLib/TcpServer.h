#pragma once
#include "AppSettings.h"
#include "Connection.h"
#include <vector>
#include "User.h"
#include "Packet.h"
#include "tsQueue.h"
#include "enumclasses.h"


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

			void NetworkFrame();
			int Console();
			int CloseConnection(int index, const std::string& reason);
			int SendToAll(Packet& packet, std::shared_ptr<Connection> ignoreClient = nullptr);
			
		public:
			TcpServer();			
			virtual ~TcpServer();			
			virtual bool onClientConnect();
			virtual bool onClientDisconnect();
			virtual bool OnUserUpdate(tsQueue<Packet>& incManager, tsQueue<Packet>& outManager, const std::vector<std::shared_ptr<Connection>>& clients);			
			int Init();
			int Start();
			void Stop();

		};
	}
}
