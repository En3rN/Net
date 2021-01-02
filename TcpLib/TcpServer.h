#pragma once
#include "AppSettings.h"
#include "Connection.h"
#include <vector>
#include "User.h"
#include "Packet.h"
#include "tsQue.h"
#include "tsVec.h"

namespace En3rN
{
	namespace Net
	{		
		class TcpServer
		{
			int			ProcessPackets();
		protected:			
			AppSettings settings;			
			tsVec<std::shared_ptr<Connection>>					connections{};
			//std::unordered_map<uint16_t, std::shared_ptr<Connection>> um;
			std::vector<WSAPOLLFD>						pollFDS{};
			bool										m_running = false;
			tsQue<Packet>								incManager{};
			tsQue<Packet>								outManager{};
			std::atomic_bool							ittConnections = false;
			//BackgroundWorker							bw;			

			int			NetworkFrame();
			int			Console();
			int			CloseConnection(int& index, const std::string& reason);			
			int			SendToAll(Packet& packet, std::shared_ptr<Connection> ignoreClient = nullptr);
			int			GetConnection(uint16_t aID);
			int			GetConnection(std::string& aUserName);

		public:
						TcpServer();			
			virtual		~TcpServer();			
			virtual int onClientConnect(std::shared_ptr<Connection> & newClient);
			virtual int onClientDisconnect(std::shared_ptr<Connection> & client);
			virtual int onUserPacket(Packet& Packet);
			virtual int onUserUpdate();
			bool		Update();
			int			SendData(Packet& packet);
			int			Init();
			int			Start();
			int			Stop();
		};
	}
}
