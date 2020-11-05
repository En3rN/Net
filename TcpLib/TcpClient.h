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
			virtual bool OnUserUpdate(tsQueue<Packet>& incManager, tsQueue<Packet>& outManager, const std::shared_ptr<Connection>& connection);
			bool NetworkFrame();
			int Console();
			int Init();
			int Start();
			void Stop();

		};
	}
}
