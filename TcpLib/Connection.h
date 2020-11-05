#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "User.h"
#include "Socket.h"
#include "Packet.h"
#include "tsQueue.h"
#include "Packet.h"
#include "IPEndpoint.h"


namespace En3rN
{
	namespace Net
	{
		class Packet;
		class Connection : public std::enable_shared_from_this<Connection>
		{	
		public:	
			enum class Type
			{Listener,Connecter,Accepted};

			Connection(Type aType, Socket&& aSocket, IPEndpoint&& aEndpoint, tsQueue<Packet>& aOutManager, tsQueue<Packet>& aIncManager);
			~Connection();
			int SendAll(Packet& packet);
			int RecvAll();
			int Connect();
			std::shared_ptr<Connection> Accept();
			int Bind();
			int Listen();
			int Disconnect(const std::string& reason);
			int Close();
			bool IsConnected() const;
			uint16_t ID();
			std::string UserName();
			pollfd& PollFD();
			Type GetType();
			void SetID(uint16_t aid);

		public:
			
			

		private:
			Type type;
			static uint16_t idCounter;
			Socket socket;
			IPEndpoint endpoint;
			uint16_t id;
			User user;
			pollfd pFd;
			tsQueue<Packet>& outPacketQue;
			tsQueue<Packet>& incPacketQue;
			bool connected = false;


		};
	}
}

