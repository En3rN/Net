#pragma once
#include "AppSettings.h"
#include <iostream>
#include <vector>
#include "User.h"
#include "Packet.h"

namespace En3rN
{
	namespace Net
	{
		class TcpClient
		{
		protected:
			AppSettings settings;
			std::shared_ptr<Connection>					connection{};			
			bool										m_running = false;


			tsQue<Packet>	incManager;
			tsQue<Packet>	outManager;			
		public:
			TcpClient();			
			virtual ~TcpClient();
			virtual int OnClientConnect(const std::shared_ptr<Connection>& connection);
			virtual int OnClientDisconnect(const std::shared_ptr<Connection>& connection);
			virtual int ProcessPackets(tsQue<Packet>& incManager, tsQue<Packet>& outManager, const std::shared_ptr<Connection>& connection);
			bool Update();
			int SendData(Packet& packet);
			int NetworkFrame();
			int Console();
			int Init();
			int Start();
			int Stop();

		};
	}
}
