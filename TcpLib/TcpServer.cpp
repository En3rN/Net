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
#include "tsQueue.h"
#include <assert.h>
#include "Packet.h"
#include <mutex>
#include "enumclasses.h"




namespace En3rN
{
    namespace Net
    {
        TcpServer::TcpServer() 
        {
            settings.ip = "0.0.0.0";                 // will accept from all ipv4
            settings.port = 50000;              // port to listen
            settings.consoleThread = true;      // console cin on a own thread
            settings.networkThread = true;      // networkthread looping in background -- accepting/ recving / sending
            settings.loop = true;               // main thread loops within run()
            settings.timeout = 5;               // timeout on wsapoll ms

            //init winsock
            logger(LogLvl::Info) << "Initializing Server";
            Network::StartupWinsock();

            //create listening connection
            IPEndpoint endP(settings.ip, settings.port);
            Socket socket(endP.GetIPVersion());
            connections.emplace_back(std::make_shared<Connection>(Connection::Type::Listener,
                std::move(socket), std::move(endP),outManager, incManager));
            if (!connections[0]->IsConnected())
            {
                logger(LogLvl::Error) << "Failed to initialize";
                return;
            }
            else
            {
                pollFDS.push_back(connections[0]->PollFD());
                m_running = true;
            }
            return;
        };
        TcpServer::~TcpServer() 
        {
            while (settings.networkThread) {}; // wait for networkThread to finish
            // close connections
            connections.clear();
            Network::ShutDownWinsock();
        };

        int TcpServer::Init()
        {
            //init winsock
            logger(LogLvl::Info) << "Initializing Server";
            Network::StartupWinsock();

            //create listening connection
            connections.emplace_back(std::make_shared<Connection>(Connection::Type::Listener, 
                Socket(),
                IPEndpoint(settings.ip, settings.port),
                outManager, incManager));
            if (!connections[0]->IsConnected())
            {
                logger(LogLvl::Error) << "Failed to initialize";                
                return -1;
            }
            else
            {
                pollFDS.push_back(connections[0]->PollFD());
                m_running = true;
            }
            return 0;
        }

        int TcpServer::ProcessPackets(tsQueue<Packet>& incManager, tsQueue<Packet>& outManager, const std::vector<std::shared_ptr<Connection>>& clients)
        {
            while (!incManager.Queue.empty())
            {
                Packet packet = std::move(incManager.PopBack());
                std::string str;
                std::vector<std::string> vStr;
                Packet response;

                //todo find out what server needs to do with msg
                logger(LogLvl::Debug) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';
                switch (packet.header.type)
                {
                case PacketType::Message:
                    packet >> str;
                    vStr= Helpers::Split(str,' ');
                    
                    if (str.find("all", 0) != str.npos)
                    {
                        str.erase(str.find("all ", 0), 4);
                        response << str;

                        SendToAll(response, packet.address);
                        break;
                    }
                    else
                    {//TODO find correct connection to send to
                        bool foundClient = false;

                        for (auto client : clients)
                        {
                            if (client->GetType() == Connection::Type::Listener) continue;
                            if (vStr[1]==client->UserName() || vStr[1] == std::to_string(client->ID()))
                            {
                                vStr.erase(vStr.begin() + 1);
                                str = Helpers::Join(vStr, ' ');
                                logger(LogLvl::Info) << packet.address->UserName() << " " << str;
                                response << str;
                                response.address = client->shared_from_this();
                                outManager << std::move(response);
                                foundClient = true;
                                break;
                            }
                        }                            
                        if (!foundClient) logger(LogLvl::Warning) << "Could not find receiver!";
                        break;
                    }                    
                    break;
                case PacketType::Command:
                    //TODO implement commands for server
                    logger(LogLvl::Warning) << "Invalid command! Deleting!";
                    break;
                default:
                    if (packet.address == nullptr)
                        logger(LogLvl::Warning) << "Unknown PackeTtype! deleting!";
                    else
                        outManager << packet;
                    break;
                }                
            }
            return 0;
        }

        int TcpServer::SendData(Packet& packet)
        {
            outManager << packet;
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
                while (ProcessPackets(incManager, outManager, connections)==0 && m_running) {}
                m_running = false;                
            }
            return 0;
        }
        int TcpServer::onClientDisconnect()
        {
            return 0;
        }
        int TcpServer::Stop()
        {   
            m_running = false;
            return 0;
        }
        int TcpServer::onClientConnect()
        {
            return 0;
        }
        int TcpServer::NetworkFrame()
        {
            int result = 0;
            int poll = 0;
            std::vector<WSAPOLLFD> useFDS = pollFDS;
            
            poll = WSAPoll(useFDS.data(), useFDS.size(), settings.timeout);
            if (poll > 0)
            {
                for (auto i = 0; i < useFDS.size(); i++)
                {
                    if (useFDS[i].revents == 0) continue;
                    
                    if(useFDS[i].revents & POLLRDNORM)
                        if (connections[i]->GetType() == Connection::Type::Listener)
                        {
                            std::shared_ptr<Connection> newConnection = connections[i]->Accept();
                            pollFDS.emplace_back(newConnection->PollFD());
                            connections.push_back(newConnection);
                            continue;
                        }
                        else
                        {
                            int r = connections[i]->RecvAll();
                            if (r < 1) { CloseConnection(i, "Recv" + std::to_string(r)); }          continue;
                        }

                    if (useFDS[i].revents & POLLERR)    { CloseConnection(i, "POLLERR");    continue; }
                    if (useFDS[i].revents & POLLHUP)    { CloseConnection(i, "POLLHUP");    continue; }
                    if (useFDS[i].revents & POLLNVAL)   { CloseConnection(i, "POLLINVAL");  continue; }
                   
                logger(LogLvl::Warning) << "Unhandled Flag! Revents: "<< useFDS[i].revents <<" On connectionID: " << connections[i]->ID();
                    
                }
            }
            if (poll == SOCKET_ERROR) logger(LogLvl::Error) << "PollErr: " << WSAGetLastError();
            

            while (outManager.Queue.size() > 0)
            {
                logger(LogLvl::Debug) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';
                Packet packet = std::move(outManager.PopBack());
                if (packet.address->SendAll(packet) != 0)
                {
                    //TODO:: decide if we want to try again
                }
            }
            if (!settings.networkThread && !settings.loop) m_running = ProcessPackets(incManager, outManager, connections);
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
                    Packet packet(connections[0]->shared_from_this());
                    v[0] = "[Server]";
                    std::string msg = Helpers::Join(v, ' ');
                    packet << msg;
                    incManager << std::move(packet);
                }
            }
            catch (const std::exception& err)
            {
                logger(LogLvl::Error) << err.what() << " Correct syntax: username intPackettype message/instruction ";
            }
                   
            
            return 0;
        }
        int TcpServer::CloseConnection(int index, const std::string& reason)
        {
            connections[index]->Disconnect(reason);            
            connections.erase(connections.begin() + index);
            pollFDS.erase(pollFDS.begin() + index);
            return 0;
        }
        int TcpServer::SendToAll(Packet& packet, std::shared_ptr<Connection> ignoreClient)
        {
            for (auto& client : connections)
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


//fd_set incSocks;
            //fd_set outSocks;
            //fd_set excSocks;

            //SOCKET incSock;
            //SOCKET outSock;
            ////SOCKET excSock;

/* incSocks = masterFDS;
                outSocks = masterFDS;
                excSocks = masterFDS;*/

                /*if (select(0, &incSocks, nullptr, nullptr, &m_settings.timeout) > 0)
                {
                    for (int i = 0; i < incSocks.fd_count; i++)
                    {
                        incSock = incSocks.fd_array[i];
                        if (incSock == m_socket && m_settings.server)
                            onClientConnect();
                        else if (incSock != NULL)
                        {
                            Packet incPacket(incSock);
                            ReceiveAll(incSock, incPacket);
                            incManager << incPacket;
                        }
                    }
                }*/

//int TcpServer::onClientDisconnect(SOCKET& socket)
    //{
    //    //drop client
    //    logger(LogLvl::Info) << socket << " : " << ConnectedUsers[GetUserID(socket)] << " has disconnected!";
    //    closesocket(socket);
    //    ConnectedUsers.erase(ConnectedUsers.begin() + GetUserID(socket));
    //    FD_CLR(socket, &masterFDS);
    //    return 0;
        


//SOCKET TcpServer::CreateSocket()
//{
//    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
//    if (s == INVALID_SOCKET)
//    {
//        logger(LogLvl::Error) << "Cant create socket";
//        Stop();
//    }
//
//    sockaddr_in hint;
//    hint.sin_family = AF_INET;
//    hint.sin_port = htons(m_settings.port);
//    // hint.sin_addr.S_un.S_addr = inet_addr(m_settings.ipAddress);
//    inet_pton(AF_INET, m_settings.ipAddress, &hint.sin_addr);
//
//
//    if (m_settings.server)
//    {   //tell winsock socket for listening
//        if (bind(s, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR)
//        {
//            logger(LogLvl::Error) << "Cant bind socket" << WSAGetLastError();
//            Stop();
//        }
//        if (listen(s, SOMAXCONN) == SOCKET_ERROR)
//        {
//            logger(LogLvl::Error) << "Cant listen @ socket" << WSAGetLastError();
//            Stop();
//        }
//    }
//    else
//    {
//        if (connect(s, (sockaddr*)&hint, sizeof(hint)) == -1)
//        {
//            logger(LogLvl::Error) << "Failed to connect to Server: " << m_settings.ipAddress << ":" << m_settings.port;
//            WSACleanup();
//            return WSAGetLastError();
//        }
//        else
//        {
//            logger(LogLvl::Info) << "Connected to " << m_settings.ipAddress << ":" << m_settings.port;
//        }
//    }
//    m_running = true;
//    return std::move(s);
//};

