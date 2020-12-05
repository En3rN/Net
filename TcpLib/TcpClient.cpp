#include "TcpClient.h"
#include "Network.h"
#include "logger.h"
#include "helpers.h"
using namespace En3rN::Net;
namespace En3rN
{
    namespace Net
    {           
        TcpClient::TcpClient()
        {
            settings.ip = "127.0.0.1";
            settings.port = 50000;
            settings.consoleThread = true;
            settings.networkThread = true;
            settings.loop = true;
            settings.timeout = 10;            
        }       

        TcpClient::~TcpClient() { Stop(); }

        int TcpClient::OnClientConnect(const std::shared_ptr<Connection>& connection)
        {
            return 0;
        }

        int TcpClient::OnClientDisconnect(const std::shared_ptr<Connection>& connection)
        {
            return 0;
        }


        int TcpClient::Console()
        {
           
            char buf[256]{};

            std::cin.getline(buf, sizeof(buf));
            std::string strBuf = buf;

            if (strBuf == "exit")
            {
                connection->Disconnect("User Exit");
                return -1;
            }
            if (strBuf == "disconnect")
            {
                connection->Disconnect("User Disconnect");
            }

            std::vector<std::string> v = Helpers::Split(strBuf, ' ');
            if (v.size() > 2)
            {
                try
                {
                    
                    Packet packet(connection->shared_from_this());
                    packet.header.type = (PacketType)stoi(v[0]);
                    v.erase(v.begin());
                    //v[0] = "[" + std::to_string(connection->ID())+"]";
                    strBuf= Helpers::Join(v, ' ');
                    packet << strBuf;
                    outManager << packet;
                    
                }
                catch (const std::exception& err)
                {
                    logger(LogLvl::Error) << err.what() << "Correct syntax: intPacketType username message/instruction ";
                }
                    
            }
            return 0;
        }

        int TcpClient::Init()
        {
            //init winsock
            logger(LogLvl::Info) << "Initializing Client";
            Network::StartupWinsock();

            //create connection
            IPEndpoint endpoint(settings.ip, settings.port);
            connection=std::make_shared<Connection>(Connection::Type::Connecter, Socket(endpoint.GetIPVersion()), std::move(endpoint),outManager, incManager);
            if (!connection->IsConnected())
            {
                logger(LogLvl::Error) << "Failed to initialize";
                settings.networkThread = false;
                settings.consoleThread = false;
                return -1;
            }
            else
                m_running = true;            
            return 0;
        }
        
        int TcpClient::ProcessPackets(tsQue<Packet>& incManager, tsQue<Packet>& outManager, const std::shared_ptr<Connection>& connection)
        {
            while (!incManager.Empty())
            {
                Packet packet = std::move(incManager.PopBack());
                std::string str;
                int i;
                //todo find out what server needs to do with msg
                logger(LogLvl::Debug) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';
                switch (packet.header.type)
                {
                case PacketType::Message:
                    packet >> str;
                    logger(LogLvl::Msg) << str;
                    break;

                case PacketType::HandShake:
                {
                    std::string key;
                    packet >> key;
                    std::string ekey = connection->Encrypt(key);
                    Packet pResponse(connection, PacketType::HandShake);
                    pResponse << ekey;
                    outManager << std::move(pResponse);
                    logger(LogLvl::Info) << "Sending handshake response!";
                    
                    break;  
                }

                case PacketType::ClientID:
                    if (packet.header.itemcount > 1)
                    {
                        logger(LogLvl::Debug) << "ClientID arr";                        
                    }
                    else
                    {
                        uint16_t id;
                        packet >> id;
                        packet.address->SetID(id);                         
                    }
                    break; 

                default:
                    logger(LogLvl::Warning) << "Unknown PacketType! deleting!";
                    break;
                }                
            }
            return 0;
        }
        bool TcpClient::Update()
        {
            while (!connection->IsValidated() && m_running)
            {
                if (!settings.networkThread) if (NetworkFrame()) return false;
                if (ProcessPackets(incManager, outManager, connection)) return false;
            }
            return m_running;
        }
        int TcpClient::SendData(Packet& packet)
        {            
            outManager << std::move(packet);

            return 0;
        }
        int TcpClient::NetworkFrame()
        {            
            int result = 0;
            int poll = 0;
            
            pollfd pFd = connection->PollFD();
            poll = WSAPoll(&pFd, 1, settings.timeout);
            if (poll > 0)
            {
                while (pFd.revents != NULL)
                {
                    if (pFd.revents & POLLRDNORM)
                    {                        
                        if (connection->RecvAll() < 1)  connection->Disconnect("Recv < 1");
                        break;
                    }
                    if (pFd.revents & POLLERR)  { connection->Disconnect("POLLERR"); break; }
                    if (pFd.revents & POLLHUP)  { connection->Disconnect("POLLHUP"); break; }
                    if (pFd.revents & POLLNVAL) { connection->Disconnect("POLLINVAL"); break; }

                    logger(LogLvl::Warning) << "Unhandled Flag! Revents: " << pFd.revents << " On connectionID: " << connection->ID();
                }

            }
            if (poll == SOCKET_ERROR)
            {
                logger(LogLvl::Error) << "PollErr: " << WSAGetLastError();
                connection->Disconnect("SocketError");                
            }
            
            while (!outManager.Empty())
            {
                logger(LogLvl::Debug) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';
                Packet packet = outManager.PopBack();                
                if (connection->SendAll(packet) != 0)
                {
                    //TODO : decide if we want to try again
                }
            }
            m_running= connection->IsConnected();
            return 0;
        }
        int TcpClient::Stop()
        {
            while (settings.networkThread) {};
            // close sockets
            connection.reset();
            //cleanup winsock
            Network::ShutDownWinsock();
            return 0;
        }

        int TcpClient::Start()
        {
            
            logger(LogLvl::Info) << "Starting Client";            

            if (settings.networkThread)
            {
                std::thread networkThread([&] {while (m_running) { NetworkFrame(); }
                    logger(LogLvl::Debug) << "NetworkThread Finished: " << std::this_thread::get_id();
                    settings.networkThread = false; });
                networkThread.detach();
            }
            if (settings.consoleThread)
            {
                std::thread consolethread([&] {while (m_running) { Console(); }
                    logger(LogLvl::Debug) << "ConsoleThread Finished: " << std::this_thread::get_id();
                    settings.consoleThread = false; });
                consolethread.detach();
            }

            if (settings.loop && m_running)
            {
                while (!ProcessPackets(incManager, outManager, connection) && m_running) {}                
                m_running = false;
            }
            return 0;
            
        };
        
    }
}