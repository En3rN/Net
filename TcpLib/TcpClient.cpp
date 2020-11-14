#include "TcpClient.h"
#include "Network.h"
#include "logger.h"
#include "helpers.h"
using namespace En3rN::Net;
namespace En3rN
{
    namespace Net
    {
        int TcpClient::onClientDisconnect()
        {
            return 0;
        }
        TcpClient::TcpClient()
        {
            settings.ip = "gafi";
            settings.port = 50000;
            settings.consoleThread = true;
            settings.networkThread = true;
            settings.loop = true;
            settings.timeout = 5;
        };

        TcpClient::~TcpClient() { Stop(); }

        int TcpClient::OnClientConnect()
        {
            return 0;
        }

        int TcpClient::OnClientDisconnect()
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
                    v[0] = "[" + std::to_string(connection->ID())+"]";
                    strBuf= Helpers::Join(v, ' ');
                    packet << strBuf;
                    outManager << packet;
                    
                }
                catch (const std::exception& err)
                {
                    logger(LogLvl::Error) << err.what() << "Correct syntax: intPackettype username message/instruction ";
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
                return -1;
            }
            else
                m_running = true;            
            return 0;
        }
        
        int TcpClient::ProcessPackets(tsQueue<Packet>& incManager, tsQueue<Packet>& outManager, const std::shared_ptr<Connection>& connection)
        {
            while (!incManager.Queue.empty())
            {
                Packet packet = std::move(incManager.PopBack());
                std::string str;
                int i;
                //todo find out what server needs to do with msg
                logger(LogLvl::Debug) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';
                switch (packet.GetPacketType())
                {
                case PacketType::Message:
                    packet >> str;
                    logger(LogLvl::Msg) << str;
                    break;
                case PacketType::Command:
                    //TODO implement commands for client
                    logger(LogLvl::Warning) << "Invalid command! Deleting!";
                    break;
                case PacketType::ClientID:
                    uint16_t id;
                    packet >> id;
                    packet.address->SetID(id);
                    break;
                default:
                    logger(LogLvl::Warning) << "Unknown PackeTtype! deleting!";

                    break;
                }                
            }
            return true;
        }
        int TcpClient::SendData(Packet& packet)
        {

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
                        int r = connection->RecvAll();
                        if (r < 1)  connection->Disconnect("Recv < 1"); 
                        break;
                    }
                    if (pFd.revents & POLLERR) { connection->Disconnect("POLLERR"); break; }
                    if (pFd.revents & POLLHUP) { connection->Disconnect("POLLHUP"); break; }
                    if (pFd.revents & POLLNVAL) { connection->Disconnect("POLLINVAL"); break; }

                    logger(LogLvl::Warning) << "Unhandled Flag! Revents: " << pFd.revents << " On connectionID: " << connection->ID();
                }

            }
            if (poll == SOCKET_ERROR)
            {
                logger(LogLvl::Error) << "PollErr: " << WSAGetLastError();
                connection->Disconnect("SocketError");                
            }
            
            while (outManager.Queue.size() > 0)
            {
                logger(LogLvl::Debug) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';
                Packet packet = outManager.PopBack();                
                if (packet.address->SendAll(packet) != 0)
                    incManager << packet; //putting packet back in que if failed to send
            }
            if (!settings.networkThread && !settings.loop) m_running = ProcessPackets(incManager, outManager, connection);   
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
                while (ProcessPackets(incManager, outManager, connection) && m_running) {}                
                m_running = false;
            }
            return 0;
            
        };
        
    }
}