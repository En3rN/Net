#include "Connection.h"
#include <fstream>
namespace En3rN
{
    namespace Net
    {

        uint16_t En3rN::Net::Connection::idCounter = 1000;

        En3rN::Net::Connection::Connection(Type aType, Socket&& aSocket, IPEndpoint&& aEndpoint, tsQue<Packet>& aOutManager,tsQue<Packet>& aIncManager) :
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
                if (bytes == 0) { logger(LogLvl::Error) << Helpers::Brackets(id) << " Sendt 0 bytes unexpected"; return -1; }
                if (bytes == SOCKET_ERROR) { logger(LogLvl::Error) << Helpers::Brackets(id) << " Socket error: " << WSAGetLastError(); return -1; }
                if (bytes == WSAEWOULDBLOCK) { logger(LogLvl::Error) << Helpers::Brackets(id) << " Socket Block: " << WSAGetLastError(); incPacketQue << packet; return -1; }
                bytesSendt += bytes;
                remainingSize -= bytes;
            }
            if (bytesSendt == packetSize)
            {
                logger(LogLvl::Debug) << "Packet Succsesfully sendt! [" << bytesSendt << "/" << packetSize << "]";
                return  0;
            }
            else
            {
                logger(LogLvl::Warning) << Helpers::Brackets(id) << " Wrong number of bytes sendt! [" << bytesSendt << "/" << packetSize << "]";
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
                    Disconnect(Helpers::Brackets(id) +" Recv 0");
                    return 0;
                }
                if (bytes == SOCKET_ERROR)
                {
                    logger(LogLvl::Error) << Helpers::Brackets(id) << " Socket ERROR: " << WSAGetLastError();
                    return WSAGetLastError();
                }
                if (bytes == WSAEWOULDBLOCK)
                {                    
                    logger(LogLvl::Error) << Helpers::Brackets(id) << " Socket Block: " << WSAGetLastError();
                    return WSAGetLastError();
                }
                bytesReceived += bytes;
            }            
            if (packet.Size() > SO_MAX_MSG_SIZE || packet.Size() < 6)
            {
                logger(LogLvl::Error) << Helpers::Brackets(id) << " Packet header mismath. Size: [" << packet.Size() << "/" << SO_MAX_MSG_SIZE << "]";
                packet.body.resize(SO_MAX_MSG_SIZE);
                remainingSize = SO_MAX_MSG_SIZE - bytesReceived;
            }
            else
            {               
                packet.body.resize(packet.Size());
                remainingSize = packet.Size();
            }
            

            while (bytesReceived < packet.Size())
            {
                remainingSize -= bytesReceived;
                bytes = recv(socket.handle, &packet.body[bytesReceived],remainingSize, 0);
                
                if (bytes == SOCKET_ERROR)
                {
                    logger(LogLvl::Error) << Helpers::Brackets(id) << " Socket ERROR: " << WSAGetLastError();
                    return WSAGetLastError();
                }
                if (bytes == WSAEWOULDBLOCK)
                {
                    logger(LogLvl::Error) << Helpers::Brackets(id) << " Socket Block: " << WSAGetLastError();
                    return WSAGetLastError();
                }
                if (bytes == 0) break;
                bytesReceived += bytes;
                
            }
            if (bytesReceived < 6) return 0; else packet.ReadHeader();
            
            if (stage > ValidationStage::NotStarted && stage<ValidationStage::Validated) Validate(packet);
            if (bytesReceived != packet.Size())
            {
                logger(LogLvl::Error) << Helpers::Brackets(id)<< " Packet header mismath. Received: [" << bytesReceived << "/" << packet.Size() << "]";
                std::fstream fs;
                while (!fs.is_open())
                {
                    fs.open("invalidPacketLog.log", std::fstream::app);
                    if (!fs.is_open())
                    {
                        fs.open("invalidPacketLog.log", std::ifstream::out);
                        fs.close();
                    }
                }
                fs << "Packet from: " << endpoint.ip << " Hostname: " << endpoint.hostname;

                for (auto c : packet.body)
                {
                    std::cout << c;
                    fs << c;
                }
                fs << "\r\n";
                fs.close();
                
                return bytes;
            }
            else
            {
                logger(LogLvl::Debug) << "Packet Succsesfully Received! [" << bytesReceived << "/" << packet.Size() << "]";
                incPacketQue << std::move(packet);
                return bytes;
            }
        }


        int En3rN::Net::Connection::Connect()
        {
            socket.SetOption(SocketOption::NonBlocking, FALSE);
            if (endpoint.GetIPVersion() == IPVersion::IPv4)
            {                
                sockaddr_in hint = endpoint.GetSockaddrIPv4();
                if (connect(socket.handle, (sockaddr*)&hint, sizeof(hint)) == -1)
                {
                    logger(LogLvl::Error) << "Connection : Connect: " << endpoint.ip << ':' << endpoint.port << " : Failed [" << WSAGetLastError() << ']';
                    return -1;
                }                
            }
            else
            {
                sockaddr_in6 hint = endpoint.GetSockaddrIPv6();
                if (connect(socket.handle, (sockaddr*)&hint, sizeof(hint)) == -1)
                {
                    logger(LogLvl::Error) << "Connection : Connect: " << endpoint.ip << ':' << endpoint.port << " : Failed [" << WSAGetLastError()<<']';                    
                    return -1;
                }
            }
            logger(LogLvl::Info) << "Connection : Connect : " << endpoint.ip << ':' << endpoint.port << " : Success";
            connected = true;            
            socket.SetOption(SocketOption::NonBlocking, TRUE);
            return 0;
        }

        std::shared_ptr<Connection> En3rN::Net::Connection::Accept()
        {
            //accept
            SOCKET newSock = INVALID_SOCKET;
            std::shared_ptr<Connection> newClient;
            if (GetIpVersion() == IPVersion::IPv4)
            {
                sockaddr_in client;
                int clientSize = sizeof(client);
                newSock = accept(socket.handle, (sockaddr*)&client, &clientSize);
                if (newSock == INVALID_SOCKET)
                {
                    logger(LogLvl::Error) << "Failed to ACCEPT [" << WSAGetLastError() << ']';
                    return nullptr;
                }
                newClient = std::make_shared<Connection>
                    (Type::Accepted, std::move(newSock), IPEndpoint((sockaddr*)&client), outPacketQue, incPacketQue);
                char host[NI_MAXHOST];
                ZeroMemory(host, NI_MAXHOST);
                if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, NULL, NULL, 0) == 0)
                    newClient->endpoint.hostname = host;
                else
                    logger(LogLvl::Error) << "getnameinfo err [" << WSAGetLastError() << ']';
            }
            
            if (GetIpVersion() == IPVersion::IPv6)
            {
                sockaddr_in6 client;
                int clientSize = sizeof(client); 
                newSock = accept(socket.handle, (sockaddr*)&client, &clientSize);
                if (newSock == INVALID_SOCKET)
                {
                    logger(LogLvl::Error) << "Failed to ACCEPT [" << WSAGetLastError() << ']';
                    return nullptr;
                }
                newClient = std::make_shared<Connection>
                    (Type::Accepted,std::move(newSock), IPEndpoint((sockaddr*)&client), outPacketQue, incPacketQue);
                //get hostname
                char host[NI_MAXHOST];
                ZeroMemory(host, NI_MAXHOST);
                if (getnameinfo((sockaddr*)&client, clientSize, host, NI_MAXHOST, NULL, NULL, 0) == 0)
                    newClient->endpoint.hostname = host;               
                else
                    logger(LogLvl::Error) << "getnameinfo err [" << WSAGetLastError() << ']';                
            }            
            logger(LogLvl::Info) << newClient->endpoint.hostname << " connected on port " << newClient->endpoint.port;
            newClient->endpoint.Print();

            //start handShake
            
            std::string key = Helpers::GenerateKey();            
            std::string encryptedKey=Encrypt(key);
            newClient->handshake = encryptedKey;
            newClient->stage = ValidationStage::Started;
            Packet phandshake(newClient, PacketType::HandShake);
            phandshake << key;
            outPacketQue << std::move(phandshake);            
            
            /*Packet IdPacket(newClient, PacketType::ClientID);
            IdPacket << newClient->id;
            outPacketQue << std::move(IdPacket);*/
            return newClient;
        }

        int En3rN::Net::Connection::Bind()
        {            

            if (endpoint.GetIPVersion() == IPVersion::IPv4)
            {
                sockaddr_in hint = endpoint.GetSockaddrIPv4();
                if (bind(socket.handle, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
                {
                    logger(LogLvl::Error) << "Connection : BIND : Failed" << WSAGetLastError();                    
                    return -1;
                }                
            }
            else
            {
                sockaddr_in6 hint = endpoint.GetSockaddrIPv6();
                socket.SetOption(SocketOption::IPv6_Only, FALSE);
                if (bind(socket.handle, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
                {
                    logger(LogLvl::Error) << "Connection : BIND : Failed [" << WSAGetLastError()<<']';                    
                    return -1;
                }               
            }

            logger(LogLvl::Info) << "Connection : BIND : " << endpoint.ip <<':'<<endpoint.port<< " : Success";            
            return 0;
        }

        int En3rN::Net::Connection::Listen()
        {
            if (listen(socket.handle, SOMAXCONN) == SOCKET_ERROR)
            {
                logger(LogLvl::Error) << "Connection : LISTEN : Failed [" << WSAGetLastError() << ']';
                return -1;
            }
            logger(LogLvl::Info) << "Connection : LISTEN : " << endpoint.ip << ':' << endpoint.port << " : Success";
            connected = true;
            return 0;
        }

        int En3rN::Net::Connection::Disconnect(const std::string& reason)
        {            
            connected = false;            
            logger(LogLvl::Info) << "Removing Connection [" << id << "] " << reason;                       
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

        bool Connection::IsValidated() const
        {
            if (stage == ValidationStage::Validated) return true;
            return false;
        }

        uint16_t & En3rN::Net::Connection::ID()
        {
            return id;
        }

        std::string& En3rN::Net::Connection::UserName()
        {

            return user.Name;
        }

        pollfd& En3rN::Net::Connection::PollFD()
        {
            return pFd;
        }
        Connection::Type Connection::GetType()
        {
            return type;
        }
        IPVersion Connection::GetIpVersion()
        {
            return endpoint.GetIPVersion();
        }
        void Connection::SetID(uint16_t aid)
        {
            logger(LogLvl::Debug) << "Setting clientID: " << aid;
            id = aid;
        }
        std::string Connection::Encrypt(std::string& data)
        {
            std::string encrypted;
            char ec{};
            for (auto c : data)
            {
                ec = c ^ '$';
                encrypted += ec;
            }
            return std::move(encrypted);
        }
        Connection::ValidationStage Connection::Validate(Packet& packet)
        {           
            if (packet.GetPacketType() != PacketType::HandShake)
            {
                connected = false;                
                return ValidationStage::NotValidated;
            }
            std::string answear;
            packet >> answear;
            if (answear != handshake)
            {
                connected = false;
                return ValidationStage::NotValidated;
            }
            else
            {                
                logger(LogLvl::Info) <<  "Client : " <<  Helpers::Brackets(packet.address->ID()) << ": Validated";
                return ValidationStage::Validated;
            }
        }
    }
}

