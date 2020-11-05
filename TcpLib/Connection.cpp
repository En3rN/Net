#include "Connection.h"
namespace En3rN
{
    namespace Net
    {

        uint16_t En3rN::Net::Connection::idCounter = 1000;

        En3rN::Net::Connection::Connection(Type aType, Socket&& aSocket, IPEndpoint&& aEndpoint, tsQueue<Packet>& aOutManager,tsQueue<Packet>& aIncManager) :
            type(aType), socket(std::move(aSocket)), endpoint(std::move(aEndpoint)),outPacketQue(aOutManager), incPacketQue(aIncManager)
        {
            id = idCounter;
            idCounter++;
            pFd = { socket.handle,POLLRDNORM,0 };
            if (type == Type::Listener) { Bind(); Listen(); }
            if (type == Type::Connecter) {Connect();}
        }

        En3rN::Net::Connection::~Connection()
        {
        }

        int En3rN::Net::Connection::SendAll(Packet& packet)
        {
            uint16_t packetSize = 0, bytesSendt = 0, remainingSize = 0, bytes = 0;

            packetSize = remainingSize = packet.Size();
            
            while (bytesSendt < packetSize)
            {
                bytes = send(socket.handle, &packet.body[bytesSendt], remainingSize, 0);
                if (bytes == 0) { logger(LogLvl::Error) << "Sendt 0 bytes unexpected"; return -1; }
                if (bytes == SOCKET_ERROR) { logger(LogLvl::Error) << "Socket error: " << WSAGetLastError(); return -1; }
                if (bytes == WSAEWOULDBLOCK) { logger(LogLvl::Error) << "Socket Block: " << WSAGetLastError(); return -1; }
                bytesSendt += bytes;
                remainingSize -= bytes;
            }
            if (bytesSendt == packetSize)
            {
                logger(LogLvl::Info) << "Packet Succsesfully sendt! [" << bytesSendt << "/" << packetSize << "]";
                return  0;
            }
            else
            {
                logger(LogLvl::Warning) << "Wrong number of bytes sendt! [" << bytesSendt << "/" << packetSize << "]";
                return 0;
            }
        }

        int En3rN::Net::Connection::RecvAll()
        {
            Packet packet(this->shared_from_this());            
            uint16_t bytesReceived = 0, bytes = 0, remainingSize=0;

            while (bytesReceived < sizeof(uint16_t))
            {
                bytes = recv(socket.handle, &packet.body[bytesReceived], sizeof(packet.header), 0);
                if (bytes == 0)
                {
                    Disconnect("Recv 0");                    
                    return 0;
                }
                if (bytes == SOCKET_ERROR)
                {
                    logger(LogLvl::Error) << "Socket ERROR: " << WSAGetLastError();
                    return WSAGetLastError();
                }
                if (bytes == WSAEWOULDBLOCK)
                {                    
                    logger(LogLvl::Error) << "Socket Block: " << WSAGetLastError();
                    return WSAGetLastError();
                }
                bytesReceived += bytes;
            }
            if (packet.Size() > SO_MAX_MSG_SIZE)
            {
                logger(LogLvl::Error) << "Packet header mismath. Size: [" << packet.Size() << "/" << SO_MAX_MSG_SIZE << "]";

            }
            packet.body.resize(packet.Size());
            remainingSize = packet.Size();

            while (bytesReceived < packet.Size())
            {
                remainingSize -= bytesReceived;
                bytes = recv(socket.handle, &packet.body[bytesReceived],remainingSize, 0);
                
                if (bytes == SOCKET_ERROR)
                {
                    logger(LogLvl::Error) << "Socket ERROR: " << WSAGetLastError();
                    return WSAGetLastError();
                }
                if (bytes == WSAEWOULDBLOCK)
                {
                    logger(LogLvl::Error) << "Socket Block: " << WSAGetLastError();
                    return WSAGetLastError();
                }
                bytesReceived += bytes;
                
            }
            if (bytesReceived != packet.Size())
            {
                logger(LogLvl::Error) << "Packet header mismath. Received: [" << bytesReceived << "/"  << packet.Size() << "]";                
                return bytes;
            }
            else
            {
                logger(LogLvl::Info) << "Packet Succsesfully Received! [" << bytesReceived << "/" << packet.Size() << "]";
                incPacketQue << std::move(packet);
                return bytes;
            }
        }


        int En3rN::Net::Connection::Connect()
        {
            if (endpoint.GetIPVersion() == IPVersion::IPv4)
            {
                socket.SetOption(SocketOption::NonBlocking, FALSE);
                sockaddr_in hint = endpoint.GetSockaddrIPv4();
                if (connect(socket.handle, (sockaddr*)&hint, sizeof(hint)) == -1)
                {
                    logger(LogLvl::Error) << "Failed to connect to Server: " << endpoint.GetIP() << ":" << endpoint.GetPort()<<" " << WSAGetLastError();
                    WSACleanup();
                    return WSAGetLastError();
                }
                else
                {
                    logger(LogLvl::Info) << "Connected to " << endpoint.GetIP() << ":" << endpoint.GetPort();
                    socket.SetOption(SocketOption::NonBlocking, TRUE);
                    connected = true;
                }
            }
            else
            {
                sockaddr_in6 hint = endpoint.GetSockaddrIPv6();
                if (connect(socket.handle, (sockaddr*)&hint, sizeof(hint)) == -1)
                {
                    logger(LogLvl::Error) << "Failed to connect to Server: " << endpoint.GetIP() << ":" << endpoint.GetPort();
                    WSACleanup();
                    return WSAGetLastError();
                }
                else
                {
                    logger(LogLvl::Info) << "Connected to " << endpoint.GetIP() << ":" << endpoint.GetPort();
                }

            }
            return 0;
        }

        std::shared_ptr<Connection> En3rN::Net::Connection::Accept()
        {
            //accept
            SOCKADDR_IN client;
            int clientSize = sizeof(client);
            Socket clientSocket(accept(socket.handle, (sockaddr*)&client, &clientSize));
            IPEndpoint clientend = IPEndpoint((sockaddr*)&client);

            std::shared_ptr<Connection> newclient = std::make_shared<Connection>(Type::Accepted, std::move(clientSocket), std::move(clientend),outPacketQue, incPacketQue);

            char host[NI_MAXHOST];
            char service[NI_MAXHOST];

            ZeroMemory(host, NI_MAXHOST);
            ZeroMemory(service, NI_MAXHOST);

            if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, service, NI_MAXHOST, 0) == 0)
                //newclient->SetHost(host);
                logger(LogLvl::Info) << host << " connected on port " << service;
            else
            {
                inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
                logger(LogLvl::Info) << host << " conncted on port " << ntohs(client.sin_port);
            }
            //welcome msg
            Packet packet(newclient);
            std::string msg = "[Server]Welcome to En3rN Server";            
            packet << msg;
            outPacketQue << std::move(packet);
            Packet pid(newclient, PacketType::ClientID);
            pid << newclient->id;
            outPacketQue << std::move(pid);
            return newclient;
        }

        int En3rN::Net::Connection::Bind()
        {

            if (endpoint.GetIPVersion() == IPVersion::IPv4)
            {
                sockaddr_in hint = endpoint.GetSockaddrIPv4();
                if (bind(socket.handle, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
                {
                    logger(LogLvl::Error) << "Connection : BIND : Failed" << WSAGetLastError();
                    WSACleanup();
                    return WSAGetLastError();
                }
                logger(LogLvl::Info) << "Connection : BIND : Success";
            }
            else
            {
                sockaddr_in6 hint = endpoint.GetSockaddrIPv6();
                if (bind(socket.handle, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
                {
                    logger(LogLvl::Error) << "Connection : BIND : Failed" << WSAGetLastError();
                    WSACleanup();
                    return WSAGetLastError();
                }
                logger(LogLvl::Info) << "Connection : BIND : Success";
            }

            return 0;
        }

        int En3rN::Net::Connection::Listen()
        {
            if (listen(socket.handle, SOMAXCONN) == SOCKET_ERROR)
            {
                logger(LogLvl::Error) << "Connection : LISTEN : Failed" << WSAGetLastError();
                return -1;
            }
            logger(LogLvl::Info) << "Connection : LISTEN : Success";
            connected = true;
            return 0;
        }

        int En3rN::Net::Connection::Disconnect(const std::string& reason)
        {            
            connected = false;            
            logger(LogLvl::Info) << "Removing Connection [" << id << "] " << reason;
            //socket.Close();            
            return 0;
        }

        int Connection::Close()
        {
            socket.Close();
            return 0;
        }

        bool En3rN::Net::Connection::IsConnected() const
        {
            return connected;
        }

        uint16_t En3rN::Net::Connection::ID()
        {
            return uint16_t(id);
        }

        std::string En3rN::Net::Connection::UserName()
        {

            return std::string(user.Name);
        }

        pollfd& En3rN::Net::Connection::PollFD()
        {
            return pFd;
        }
        Connection::Type Connection::GetType()
        {
            return type;
        }
        void Connection::SetID(uint16_t aid)
        {
            logger(LogLvl::Debug) << "Setting clientID: " << aid;
            id = aid;
        }
    }
}

