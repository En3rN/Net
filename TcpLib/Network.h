#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>

namespace En3rN
{
	namespace Net
	{
		class Network
		{
		public:
			static int StartupWinsock();
			static int ShutDownWinsock();
		};
	}
}

