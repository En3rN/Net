#include "Network.h"
#include "TcpServer.h"
#include "Socket.h"
#include "IPEndpoint.h"
#include <vector>
#include <thread>
#include <iostream>
#include "helpers.h"
#include "logger.h"
#include "User.h"
#include "tsQue.h"
#include <assert.h>
#include "Packet.h"
#include <mutex>

namespace En3rN
{
    namespace Net
    {
        TcpServer::TcpServer()
        {
            settings.ip = "::";                 // will accept from all ipv6
            settings.port = 50000;              // port to listen
            settings.consoleThread = true;      // console cin on a own thread
            settings.networkThread = true;      // networkthread looping in background -- accepting/ recving / sending
            settings.loop = true;               // main thread loops within run()
            settings.timeout = 5;               // timeout on wsapoll ms
            connections.reserve(1000);            
        }
        TcpServer::~TcpServer() 
        {            
            while (settings.networkThread || settings.consoleThread) 
            {
                std::this_thread::yield();
            }; // wait for networkThread to finish
            // close connections
            connections.clear();
            Network::ShutDownWinsock();
        };

        bool TcpServer::Update()
        {            
            if (!settings.networkThread) if(!NetworkFrame()) return false;
            if(ProcessPackets()) return false;
            return m_running;
        }

        int TcpServer::Init()
        {
            //init winsock
            logger(LogLvl::Info) << "Initializing Server";
            Network::StartupWinsock();

            //create listening connection
            IPEndpoint endP(settings.ip, settings.port);
            Socket socket(endP.GetIPVersion());
            connections.emplace_back(std::make_shared<Connection>(Connection::Type::Listener,
                std::move(socket), std::move(endP), outManager, incManager));
            if (!connections[0]->IsConnected())
            {
                logger(LogLvl::Error) << "Failed to initialize";
                settings.networkThread = false;
                settings.consoleThread = false;
                return -1;
            }
            else
            {
                pollFDS.push_back(connections[0]->PollFD());
                m_running = true;
            }
            return 0;
        }

        int TcpServer::ProcessPackets()
        {
            while (!incManager.Empty())
            {
                Packet packet = std::move(incManager.PopBack());
                std::string str;
                std::vector<std::string> vStr;
                Packet response;

                logger(LogLvl::Debug) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';

                switch (packet.GetPacketType())
                {
                case PacketType::Message:

                    packet >> str;
                    vStr = Helpers::Split(str, ' ');

                    if (vStr[0]=="all")
                    {
                        if (packet.address != nullptr)
                            vStr[0] = Helpers::Brackets(packet.address->ID());
                        else
                            vStr.erase(vStr.begin());
                        
                        response << Helpers::Join(vStr,' ');

                        SendToAll(response, packet.address);
                        break;
                    }
                    else
                    {//TODO find correct connection to send to
                        bool foundClient = false;

                        connections.mtx.lock();
                        for (auto client : connections)
                        {
                            if (client->GetType() == Connection::Type::Listener) continue;
                            if (vStr[0] == client->UserName() || vStr[0] == std::to_string(client->ID()))
                            {
                                if (packet.address != nullptr)
                                    vStr[0] = Helpers::Brackets(packet.address->ID());
                                else
                                    vStr.erase(vStr.begin());
                                str = Helpers::Join(vStr, ' ');
                                //logger(LogLvl::Info) << packet.address->UserName() << " " << str;
                                response << str;
                                response.address = client->shared_from_this();
                                outManager << std::move(response);
                                foundClient = true;
                                break;
                            }
                        }
                        connections.mtx.unlock();
                        /*ittConnections = false;*/
                        if (!foundClient) logger(LogLvl::Warning) << "Could not find receiver!";
                        break;
                    }
                    break;

                case PacketType::HandShake:
                    //packet.address->Validate(packet);
                    break;
                default:
                    if(onMessage(packet)!=0) return 1;
                    break;
                }
            }
        return 0;
        }
        int TcpServer::onMessage(Packet& Packet)
        {
            return 0;
        }

        int TcpServer::SendData(Packet& packet)
        {
            incManager << packet;
            return 0;
        }

        int TcpServer::Start()
        {
            logger(LogLvl::Info) << "Starting Server";            

            if (settings.networkThread)
            {
                std::thread networkThread([&]{while (m_running){NetworkFrame();}
                    logger(LogLvl::Debug) << "NetworkThread Finished: " << std::this_thread::get_id();
                    settings.networkThread = false;});
                networkThread.detach();
            }
            if (settings.consoleThread)
            {
                std::thread consolethread([&]{while (m_running){Console();}
                    logger(LogLvl::Debug) << "ConsoleThread Finished: " << std::this_thread::get_id();
                    settings.consoleThread = false; });                
                consolethread.detach();
            }

            if (settings.loop && m_running)
            {
                while (ProcessPackets()==0 && m_running) {}
                m_running = false;                
            }
            
            return 0;
        }
        int TcpServer::onClientDisconnect(std::shared_ptr<Connection> client)
        {
            return 0;
        }
        int TcpServer::Stop()
        {   
            m_running = false;
            return 0;
        }
        int TcpServer::onClientConnect(std::shared_ptr<Connection> newClient)
        {
            Packet packet(newClient);
            std::string msg = "[Server]Welcome to En3rN Server";
            packet << msg;
            outManager << std::move(packet);

            return 0;
        }
        int TcpServer::NetworkFrame()
        {
            int result = 0;
            int poll = 0;
            //std::vector<WSAPOLLFD> useFDS = pollFDS;
            
            poll = WSAPoll(pollFDS.data(), (ULONG)pollFDS.size(), settings.timeout);
            if (poll > 0)
            {
                for (auto i = 0; i < pollFDS.size(); i++)
                {                    
                    if (pollFDS[i].revents == 0) continue;
                    
                    if(pollFDS[i].revents & POLLRDNORM)
                        if (connections[i]->GetType() == Connection::Type::Listener)
                        {
                            std::shared_ptr<Connection> newConnection = connections[i]->Accept();                            
                            if (onClientConnect(newConnection)==0)
                            {
                                pollFDS.emplace_back(newConnection->PollFD());
                                connections.PushBack(newConnection);
                                Packet clientIds(nullptr, PacketType::ClientID);
                                for (int i = 1; i < connections.size(); i++)
                                {
                                    clientIds << connections[i]->ID();
                                }
                            }
                            continue;
                        }
                        else
                        {                            
                            int r = connections[i]->RecvAll();
                            if(r==-10)  { CloseConnection(i, "Failed Validation"); } continue;
                            if (r < 1)  { CloseConnection(i, "Recv : " + std::to_string(r)); } continue;
                        }
                    
                    if (pollFDS[i].revents & POLLERR)    { CloseConnection(i, "POLLERR");    continue; }
                    if (pollFDS[i].revents & POLLHUP)    { CloseConnection(i, "POLLHUP");    continue; }
                    if (pollFDS[i].revents & POLLNVAL)   { CloseConnection(i, "POLLINVAL");  continue; }
                    
                   
                logger(LogLvl::Warning) << "Unhandled Flag! Revents: "<< pollFDS[i].revents <<" On connectionID: " << connections[i]->ID();
                    
                }
            }
            if (poll == SOCKET_ERROR) logger(LogLvl::Error) << "PollErr: " << WSAGetLastError();
            

            while (!outManager.Empty())
            {
                logger(LogLvl::Debug) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';
                Packet packet = std::move(outManager.PopBack());
                if (packet.address->SendAll(packet) != 0)
                {
                    //TODO:: decide if we want to try again
                }
            }
            return 0;
        }
        int TcpServer::Console()
        {            
            char buf[256]{};

            std::cin.getline(buf, sizeof(buf));
            std::string strBuf = buf;
            std::cin.clear();
            if (strBuf == "exit")
            {
                m_running = false;
                return -1;
            }

            std::vector<std::string> v = Helpers::Split(strBuf, ' ');
                
            try
            {                
                if ((PacketType)std::stoi(v[0]) == PacketType::Message)
                {
                    Packet packet; 
                    v.erase(v.begin());
                    v.insert(v.begin() + 1,"[Server]");
                    std::string msg = Helpers::Join(v, ' ');                    
                    packet << msg;
                    std::string str;                    
                    incManager << std::move(packet);
                }
            }
            catch (const std::exception& err)
            {
                logger(LogLvl::Error) << err.what() << " Correct syntax: intPacketType  username/VlientID message/instruction ";
            }
                   
            
            return 0;
        }
        int TcpServer::CloseConnection(int& index, const std::string& reason)
        {
            /*while (ittConnections) 
            {
                logger(LogLvl::Warning) << "Waiting for itt!!";
            };*/
            std::scoped_lock lock(connections.mtx);
            connections[index]->Disconnect(reason);
            onClientDisconnect(connections[index]);
            connections.erase(connections.begin() + index);
            pollFDS.erase(pollFDS.begin() + index);
            index--;
            return 0;
        }
        int TcpServer::SendToAll(Packet& packet, std::shared_ptr<Connection> ignoreClient)
        {
            std::scoped_lock lock(connections.mtx);
            for (auto client : connections)
            {
                if (client->GetType() != Connection::Type::Listener && client!=ignoreClient)
                {
                    packet.address = client->shared_from_this();
                    outManager << packet;
                }
            }
            return 0;
        }
        int TcpServer::GetConnection(uint16_t aID)
        {
            return 0;
        }
        int TcpServer::GetConnection(std::string& aUserName)
        {
            return 0;
        }
    }
}