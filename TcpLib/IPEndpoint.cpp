#include "IPEndpoint.h"
#include "helpers.h"
#include <assert.h>
#include <string>

namespace En3rN
{
	namespace Net
	{		
		IPEndpoint::IPEndpoint(const char* aIp, unsigned short aPort) :ip(aIp), hostname(aIp), port(aPort)
		{			
			in_addr addr;
			
			if (inet_pton(AF_INET, aIp, &addr) == 1)
			{
				if (addr.S_un.S_addr != INADDR_NONE)
				{					
					ipversion = IPVersion::IPv4;
					return;
				}
			}

				
			addrinfo hints = {};
			hints.ai_family = AF_INET;				
			addrinfo* hostinfo = nullptr;				
								

			if (getaddrinfo(aIp, NULL, &hints, &hostinfo) == 0)
			{
				sockaddr_in* host_addr = reinterpret_cast<sockaddr_in*> (hostinfo->ai_addr);
				ip.resize(16);
				inet_ntop(AF_INET, &host_addr->sin_addr, &ip[0], 16);
				//Helpers::trim(ip);				
				ipversion = IPVersion::IPv4;
				freeaddrinfo(hostinfo);
				return;
			}
			
			//IPv6
			in6_addr addr6;
			if (inet_pton(AF_INET6, aIp, &addr6) == 1)
			{				
				ipversion = IPVersion::IPv6;
				return;
			}

			//Attempt to resolve hostname to ipv6 address
			addrinfo hintsv6 = {}; //hints will filter the results we get back for getaddrinfo
			hintsv6.ai_family = AF_INET6; //ipv6 addresses only
			addrinfo* hostinfov6 = nullptr;

			if (getaddrinfo(aIp, NULL, &hintsv6, &hostinfov6) == 0)
			{
				sockaddr_in6* host_addr = reinterpret_cast<sockaddr_in6*>(hostinfov6->ai_addr);
				ip.resize(46);
				inet_ntop(AF_INET6, &host_addr->sin6_addr, &ip[0], 46);
				//Helpers::trim(ip);					
				ipversion = IPVersion::IPv6;
				freeaddrinfo(hostinfov6);
				return;
			}
			

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
				//Helpers::trim(ip);
				hostname = ip;
			}
			else //IPv6
			{
				sockaddr_in6* addrv6 = reinterpret_cast<sockaddr_in6*>(aAddr);
				ipversion = IPVersion::IPv6;
				port = ntohs(addrv6->sin6_port);
				ip.resize(46);
				inet_ntop(AF_INET6, &addrv6->sin6_addr, &ip[0], 46);
				//Helpers::trim(ip);				
				hostname = ip;
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
			//std::string ipBytes = Helpers::Filter(ip, '.');
			inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
			//memcpy(&addr.sin_addr, &ipBytes[0], sizeof(ULONG));
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
				std::cout << "IP Version: IPv4" << std::endl;
				break;
			case IPVersion::IPv6:
				std::cout << "IP Version: IPv6" << std::endl;
				break;
			default:
				std::cout << "IP Version: Unknown" << std::endl;
			}
			std::cout << "Hostname: " << hostname << std::endl;
			std::cout << "IP: " << ip << std::endl;
		}
	}
}
