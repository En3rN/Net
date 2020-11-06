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


			tsQueue<Packet>	incManager;
			tsQueue<Packet>	outManager;
			int onClientDisconnect();
		public:
			TcpClient();
			virtual ~TcpClient();
			virtual int OnClientConnect();
			virtual int OnClientDisconnect();
			virtual int ProcessPackets(tsQueue<Packet>& incManager, tsQueue<Packet>& outManager, const std::shared_ptr<Connection>& connection);			
			int SendData(Packet& packet);
			int NetworkFrame();
			int Console();
			int Init();
			int Start();
			int Stop();

		};
	}
}
