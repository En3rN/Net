
#include "Socket.h"
#include "logger.h"


namespace En3rN
{
    namespace Net
    {  
        Socket::Socket(SocketHandle&& aHandle) :  handle(std::move(aHandle))
        {            
            aHandle = INVALID_SOCKET;
            Create();
        }

        Socket::Socket(IPVersion ipVersion) 
        {            
            handle = INVALID_SOCKET;
            Create(ipVersion);
        }

        /*Socket::Socket(Socket&& other)
        {
            logger::ScopedSettings loggerSettings = logger::ScopedSettings(LogLvl::Debug, true);
            ipVersion = other.ipVersion;
            handle = std::move(other.handle);
            other.handle = INVALID_SOCKET;
        }*/

        Socket& Socket::operator=(Socket&& other) noexcept
        {
            Close();            
            handle = std::move(other.handle);
            other.handle = INVALID_SOCKET;
            return *this;
        }


        Socket::Socket(Socket&& other) noexcept
        {            
            handle = std::move(other.handle);
            other.handle = INVALID_SOCKET;
        }

        Socket::~Socket() 
        {            
            if (handle != INVALID_SOCKET)
                logger(LogLvl::Info) << "Closing Socket [" << handle << "] " << closesocket(handle) << " Error: " << WSAGetLastError();
        }

        int Socket::SetOption(SocketOption option, BOOL value)
        {            
            int result(0);
            u_long iMode = value;
            switch (option)
            {
            case SocketOption::TCP_Nodelay:
                result = setsockopt(handle, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
                break;
            case SocketOption::IPv6_Only:
                result = setsockopt(handle, IPPROTO_IPV6, IPV6_V6ONLY, (const char*)&value, sizeof(value));
                break;
            case SocketOption::NonBlocking:                
                result = ioctlsocket(handle, FIONBIO, &iMode);
                break;
            default:
                result = -1;
                break;
            }
            if (result != 0)
            {
                logger(LogLvl::Error) << "Error setting options! Option[" << (int)option << ','<< value<< "]["<< WSAGetLastError()<< ']';
                return result;
            }
            return 0;
        }
        

        int Socket::Create(IPVersion IpVersion)
        {
            if (handle == INVALID_SOCKET)
            {
                handle = socket((IpVersion == IPVersion::IPv4) ? AF_INET : AF_INET6, SOCK_STREAM, IPPROTO_TCP);
                if (handle == INVALID_SOCKET)
                {
                    logger(LogLvl::Error) << "Error creating socket";
                    return -1;
                }
            }
            logger(LogLvl::Debug) << "Socket created!";
            SetOption(SocketOption::TCP_Nodelay, TRUE);
            SetOption(SocketOption::NonBlocking, TRUE);            
            return 0;
        }

        int Socket::Close()
        {
            if (handle != SOCKET_ERROR)
            {
                int r = closesocket(handle);
                logger(LogLvl::Debug) << "Closing Socket: " << socket << ":" << r;
            }
            return 0;
        }
    }
}
