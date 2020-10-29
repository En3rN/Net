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
            settings.ip = "127.1.1.1";
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


        int TcpClient::Console()
        {
           
            char buf[256]{};

            std::cin.getline(buf, sizeof(buf));
            std::string strBuf = buf;

            if (strBuf == "exit")
            {
                m_running = false;
                return -1;
            }
            if (strBuf == "disconnect")
            {
                connection->Disconnect();
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
            connection=std::make_shared<Connection>(Connection::Type::Connecter, std::move(Socket()), IPEndpoint(settings.ip, settings.port), incManager);
            if (!connection->IsConnected())
            {
                logger(LogLvl::Error) << "Failed to initialize";
                return -1;
            }
            else
                m_running = true;            
            return 0;
        }
        
        bool TcpClient::OnUserUpdate(tsQueue<Packet>& incManager, tsQueue<Packet>& outManager, const std::shared_ptr<Connection>& connection)
        {
            while (!incManager.Queue.empty())
            {
                Packet packet = incManager.PopBack();
                std::string str;
                int i;
                //todo find out what server needs to do with msg
                logger(LogLvl::Info) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';
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
                    packet.owner->SetID(id);
                    break;
                default:
                    logger(LogLvl::Warning) << "Unknown PackeTtype! deleting!";
                    
                    break;
                }
                logger(LogLvl::Info) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';
            }
        return true;
    }
        void TcpClient::NetworkFrame()
        {
            int result = 0;
            int poll = 0;
           
            poll = WSAPoll(&connection->PollFD(), 1, settings.timeout);
            if (poll > 0) connection->RecvAll();
            
            while (outManager.Queue.size() > 0)
            {
                logger(LogLvl::Debug) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';
                Packet packet = outManager.PopBack();                
                if (packet.owner->SendAll(packet) != 0)
                    incManager << packet; //putting packet back in que if failed to send
            }
            if (!settings.networkThread && !settings.loop) m_running = OnUserUpdate(incManager, outManager, connection);
        }
        void TcpClient::Stop()
        {
            while (settings.networkThread) {};
            // close sockets
            connection->Close();
            //cleanup winsock
            Network::ShutDownWinsock();
        }

        int TcpClient::Start()
        {
            
            logger(LogLvl::Info) << "Starting Client";
            if (m_running != true) Stop();

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
                while (OnUserUpdate(incManager, outManager, connection) && m_running) {}                
                m_running = false;
            }
            return 0;
            
        };        
    }
}