#pragma once
#include "Network.h"
#include "AppSettings.h"
#include <iostream>
#include <vector>
#include "User.h"
#include "Connection.h"
#include "tsQue.h"
#include "Packet.h"

namespace En3rN
{
	namespace Net
	{
		class TcpClient
		{
			int ProcessPackets();
			
		public:
			TcpClient();			
			virtual ~TcpClient();
			virtual int onClientConnect(const std::shared_ptr<Connection> & connection);
			virtual int onClientDisconnect(const std::shared_ptr<Connection> & connection);
			virtual int onUserPacket(Packet& Packet);
			virtual int onUserUpdate();
			
			
			
			bool Update();
			int SendData(Packet& packet);
			int NetworkFrame();
			int Console();
			int Init();
			int Start();
			int Stop();

		protected:
			AppSettings settings;
			std::shared_ptr<Connection>							connection{};
			bool										m_running = false;


			tsQue<Packet>	incManager;
			tsQue<Packet>	outManager;

		};
	}
}
