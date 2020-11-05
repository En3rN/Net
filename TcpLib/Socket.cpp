
#include "Socket.h"
#include "logger.h"


namespace En3rN
{
    namespace Net
    {  
        Socket::Socket(SocketHandle&& aHandle) :  handle(std::move(aHandle))
        {
            logger::ScopedSettings loggerSettings = logger::ScopedSettings(LogLvl::Debug, true);
            aHandle = INVALID_SOCKET;
            Create();
        }

        Socket::Socket(IPVersion aIpv, SocketHandle&& aHandle) : ipVersion(aIpv), handle(std::move(aHandle))
        {
            logger::ScopedSettings loggerSettings = logger::ScopedSettings(LogLvl::Debug, true);
            aHandle = INVALID_SOCKET;
            Create();
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
            logger::ScopedSettings loggerSettings = logger::ScopedSettings(LogLvl::Debug, true);
            Close();
            ipVersion = std::move(other.ipVersion);
            handle = std::move(other.handle);
            other.handle = INVALID_SOCKET;
            return *this;
        }


        Socket::Socket(Socket&& other) noexcept
        {
            logger::ScopedSettings loggerSettings = logger::ScopedSettings(LogLvl::Debug, true);
            ipVersion = std::move(other.ipVersion);
            handle = std::move(other.handle);
            other.handle = INVALID_SOCKET;
        }

        Socket::~Socket() 
        {
            logger::ScopedSettings loggerSettings = logger::ScopedSettings(LogLvl::Debug, true);
            if (handle != INVALID_SOCKET)
            {
                logger(LogLvl::Debug) << "Closing Socket [" << handle << "] " << closesocket(handle) << " Err: " << WSAGetLastError();
                
            }

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
                logger(LogLvl::Error) << "Error setting options!" << WSAGetLastError();
                return result;
            }
            return 0;
        }
        

        int Socket::Create()
        {
            if (handle == INVALID_SOCKET)
            {
                handle = socket((ipVersion == IPVersion::IPv4) ? AF_INET : AF_INET6, SOCK_STREAM, IPPROTO_TCP);
                if (handle == INVALID_SOCKET)
                {
                    logger(LogLvl::Error) << "Error creating socket";
                    return -1;
                }
                logger(LogLvl::Info) << "Socket created!";
                SetOption(SocketOption::TCP_Nodelay, TRUE);
                SetOption(SocketOption::NonBlocking, TRUE);
                if(ipVersion==IPVersion::IPv6) SetOption(SocketOption::IPv6_Only, FALSE);
            }
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
