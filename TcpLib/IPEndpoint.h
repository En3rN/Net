#pragma once
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>

namespace En3rN
{
	namespace Net
	{
		enum class IPVersion
		{
			IPv4,IPv6
		};

		class IPEndpoint
		{
		public:
			IPEndpoint() {};
			IPEndpoint(const char* aip, unsigned short aport);
			IPEndpoint(sockaddr* addr);
			IPVersion GetIPVersion();
			std::string GetHostname();
			std::string GetIP();
			uint16_t  GetPort();
			sockaddr_in GetSockaddrIPv4();
			sockaddr_in6 GetSockaddrIPv6();
			void Print();
		private:
			IPVersion ipversion = IPVersion::IPv4;
			std::string ip = "0.0.0.0";
			std::string hostname = "";			
			uint16_t  port = 50000;
		};
	}
}
