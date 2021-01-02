#pragma once
#include "Network.h"
#include <string>

namespace En3rN
{
	namespace Net
	{
		enum class IPVersion
		{
			Unknown,IPv4,IPv6
		};

		class IPEndpoint
		{
			friend class Connection;
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
			void SetHostname();
		private:

			IPVersion ipversion=IPVersion::Unknown;
			std::string ip = "";
			std::string hostname = "";			
			uint16_t port = 0;
		};
	}
}

