#include "Network.h"
#include "logger.h"
namespace En3rN
{
    namespace Net
    {
        int Network::StartupWinsock()
        {
            logger(LogLvl::Info) << "Starting Winsock";
            WSADATA wsaData;
            
            int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (result != 0)
            {
                logger(LogLvl::Error) << "Failed to startup Winsock: " << WSAGetLastError();
                WSACleanup();
            }
            return result;
        }
        int Network::ShutDownWinsock()
        {
            //cleanup winsock
            int r = WSACleanup();
            logger(LogLvl::Info) << "WSACleanup: " << r;
            return r;
        }
    }

}