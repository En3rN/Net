#pragma once
#include "enumclasses.h"
#include "IPEndpoint.h"

typedef SOCKET SocketHandle;
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
		public:	
			Socket(SocketHandle&& aHandle);
			Socket(IPVersion ipv=IPVersion::IPv4, SocketHandle&& handle =INVALID_SOCKET);
			//Socket(Socket&& other);
			Socket& operator=(Socket&& other) noexcept;
			Socket(Socket&& other) noexcept;
			~Socket();
		
			int SetOption(SocketOption option, BOOL value);
			int Create();
			int Close();
			
			IPVersion ipVersion;
			SocketHandle handle;
		};
	}
}

