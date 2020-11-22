#include "IPEndpoint.h"
#include "helpers.h"
#include <assert.h>
#include <string>
#include "logger.h"

namespace En3rN
{
	namespace Net
	{
		IPEndpoint::IPEndpoint(const char* aIp, unsigned short aPort) : ip(aIp), hostname(aIp), port(aPort)
		{
			char host[NI_MAXHOST];
			ZeroMemory(host, NI_MAXHOST);
			char ipstringbuffer[46];
			DWORD ipbufferlength = 46;
			int i = 1;
			in_addr addrv4;
			in6_addr addrv6;			
			
			if (inet_pton(AF_INET, aIp, &addrv4) == 1)
			{
				ipversion = IPVersion::IPv4;
				ip.resize(16);
				if (inet_ntop(AF_INET, &addrv4, &ip[0], 16) == NULL)
					logger(LogLvl::Error) << "inet_ntopErr [" << WSAGetLastError() << ']';				
				return;
			}

			addrinfo hints = {}; //hints will filter the results we get back for getaddrinfo
			hints.ai_family = AF_INET; //ipv4 addresses only
			addrinfo* hostinfo = nullptr;

			if (getaddrinfo(aIp, NULL, &hints, &hostinfo) == 0)
			{
				ipversion = IPVersion::IPv4;
				sockaddr_in* host_addr = reinterpret_cast<sockaddr_in*> (hostinfo->ai_addr);
				ip.resize(16);
				if (inet_ntop(AF_INET, &host_addr->sin_addr, &ip[0], 16) == NULL)
					logger(LogLvl::Error) << "inet_ntopErr [" << WSAGetLastError() << ']';
				Helpers::trim(ip);				
				freeaddrinfo(hostinfo);					
				return;
			}
			
			//IPv6
			in6_addr addr6;
			if (inet_pton(AF_INET6, aIp, &addr6) == 1)
			{
				ipversion = IPVersion::IPv6;
				ip.resize(46);
				if (inet_ntop(AF_INET6, &addr6, &ip[0], 46) == NULL)
					logger(LogLvl::Error) << "inet_ntopErr [" << WSAGetLastError() << ']';				
				return;
			}
			//Attempt to resolve hostname to ipv6 address
			addrinfo hintsv6 = {}; //hints will filter the results we get back for getaddrinfo
			hintsv6.ai_family = AF_INET6; //ipv6 addresses only
			hintsv6.ai_flags = AI_ALL;
			addrinfo* hostinfov6 = nullptr;

			if (getaddrinfo(aIp, NULL, &hintsv6, &hostinfov6) == 0)
			{
				ipversion = IPVersion::IPv6;
				sockaddr_in6* host_addr = reinterpret_cast<sockaddr_in6*>(hostinfov6->ai_addr);
				ip.resize(46);
				if (inet_ntop(AF_INET6, &host_addr->sin6_addr, &ip[0], 46) == NULL)
					logger(LogLvl::Error) << "inet_ntopErr [" << WSAGetLastError() << ']';
				Helpers::trim(ip);				
				freeaddrinfo(hostinfov6);					
				return;
			}
			
			if (ipversion == IPVersion::Unknown) logger(LogLvl::Error) << "Failed to determine IpVersion!";
		}

		IPEndpoint::IPEndpoint(sockaddr* aAddr)
		{
			

			if (aAddr->sa_family == AF_INET) //IPv4
			{
				sockaddr_in* addrv4 = reinterpret_cast<sockaddr_in*>(aAddr);
				ipversion = IPVersion::IPv4;
				port = ntohs(addrv4->sin_port);
				ip.resize(16);
				inet_ntop(AF_INET, &addrv4->sin_addr, &ip[0], 16);							
				Helpers::trim(ip);				
			}
			else //IPv6
			{
				sockaddr_in6* addrv6 = reinterpret_cast<sockaddr_in6*>(aAddr);
				ipversion = IPVersion::IPv6;
				port = ntohs(addrv6->sin6_port);				
				ip.resize(46);
				inet_ntop(AF_INET6, &addrv6->sin6_addr, &ip[0], 46);								
				Helpers::trim(ip);				
			}
		}

		IPVersion IPEndpoint::GetIPVersion()
		{
			return IPVersion(ipversion);
		}
		std::string IPEndpoint::GetHostname()
		{
			return std::string(hostname);
		}
		std::string IPEndpoint::GetIP()
		{
			return std::string(ip);
		}
		uint16_t IPEndpoint::GetPort()
		{
			return uint16_t(port);
		}
		sockaddr_in IPEndpoint::GetSockaddrIPv4()
		{
			assert(ipversion == IPVersion::IPv4);
			sockaddr_in addr = {};
			addr.sin_family = AF_INET;			
			inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);			
			addr.sin_port = htons(port);
			return addr;
		}

		sockaddr_in6 IPEndpoint::GetSockaddrIPv6()
		{
			assert(ipversion == IPVersion::IPv6);
			sockaddr_in6 addr = {};
			addr.sin6_family = AF_INET6;
			inet_pton(AF_INET6, ip.c_str(), &addr.sin6_addr);			
			addr.sin6_port = htons(port);
			return addr;
		}

		void IPEndpoint::Print()
		{
			switch (ipversion)
			{
			case IPVersion::IPv4:
				std::cout << "\tIP Version: IPv4" << std::endl;
				break;
			case IPVersion::IPv6:
				std::cout << "\tIP Version: IPv6" << std::endl;
				break;
			default:
				std::cout << "\tIP Version: Unknown" << std::endl;
			}
			std::cout << "\tHostname: " << hostname << std::endl;
			std::cout << "\tIP: " << ip << std::endl;
			std::cout << "\tPort: " << port << std::endl;
		}
		void IPEndpoint::SetHostname()
		{
			char host[NI_MAXHOST];
			ZeroMemory(host, NI_MAXHOST);
		
			sockaddr client;
			size_t clientSize = ipversion == IPVersion::IPv4 ? sizeof(GetSockaddrIPv4()) : sizeof(GetSockaddrIPv6());
			client.sa_family = ipversion == IPVersion::IPv4 ? AF_INET : AF_INET6;
			
			if (inet_pton(client.sa_family, ip.c_str(), &client.sa_data) == 1)
			{
				
					
				
			}
			logger(LogLvl::Error) << "inet_pton err [" << WSAGetLastError() << ']';
		}
	}
}
