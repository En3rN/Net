#pragma once
#include "Network.h"
#include "IPEndpoint.h"

using SocketHandle = SOCKET;
namespace En3rN
{
	namespace Net
	{
		enum class SocketOption
		{
			TCP_Nodelay, IPv6_Only, NonBlocking
		};

		

		class Socket
		{
			friend class Connection;

		public:	
			Socket(SocketHandle&& aHandle);
			Socket(IPVersion ipVersion=IPVersion::IPv4);			
			Socket& operator=(Socket&& other) noexcept;
			Socket(Socket&& other) noexcept;
			~Socket();

		private:		
			int SetOption(SocketOption option, BOOL value);
			int Create(IPVersion ipVersion= IPVersion::Unknown);
			int Close();
			
			SocketHandle handle;
		};
	}
}

