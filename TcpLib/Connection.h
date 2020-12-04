#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "User.h"
#include "Socket.h"
#include "Packet.h"
#include "tsQue.h"
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
			enum class Type {Listener,Connecter,Accepted};
			enum class ValidationStage {NotStarted,Started,NotValidated,Validated};

			Connection(Type aType, Socket&& aSocket, IPEndpoint&& aEndpoint, tsQue<Packet>& aOutManager, tsQue<Packet>& aIncManager);
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
			bool IsValidated() const;
			uint16_t& ID();
			std::string& UserName();
			pollfd& PollFD();
			Type GetType();
			IPVersion GetIpVersion();
			void SetID(uint16_t aid);
			std::string Encrypt(std::string& data);
			ValidationStage Validate(Packet& packet);

		private:
			Type type;
			static uint16_t idCounter;
			Socket socket;
			IPEndpoint endpoint;
			uint16_t id;
			User user;
			pollfd pFd;
			tsQue<Packet>& outPacketQue;
			tsQue<Packet>& incPacketQue;
			bool connected = false;
			ValidationStage stage;
			std::string handshake = "";
		};
	}
}

