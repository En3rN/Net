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
            settings.ip = "0.0.0.0";
            settings.port = 50000;
            settings.consoleThread = true;
            settings.networkThread = true;
            settings.loop = true;
            settings.timeout = 5;
        };
        TcpServer::~TcpServer() { Stop(); };

        int TcpServer::Init()
        {
            //init winsock
            logger(LogLvl::Info) << "Initializing Server";
            Network::StartupWinsock();

            //create listening connection
            connections.emplace_back(std::make_shared<Connection>(Connection::Type::Listener, std::move(Socket()),IPEndpoint(settings.ip, settings.port), incManager));
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

        bool TcpServer::OnUserUpdate(tsQueue<Packet>& incManager, tsQueue<Packet>& outManager, const std::vector<std::shared_ptr<Connection>>& clients)
        {
            while (!incManager.Queue.empty())
            {
                Packet packet = incManager.Back();
                std::string str;
                //todo find out what server needs to do with msg
                logger(LogLvl::Info) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';
                switch (packet.header.type)
                {
                case PacketType::Message:                     
                    if (packet.owner->GetType() != Connection::Type::Listener)
                    {
                        incManager >> outManager;
                    }
                    else if (packet.body.find("all", 6) != str.npos)
                    {
                        for (auto client : clients)
                        {
                            if (client->GetType() != Connection::Type::Listener)
                            {
                                packet = incManager.Back();
                                packet.owner = client->shared_from_this();
                                outManager << packet;
                            }
                        }
                        incManager.PopBack();
                        break;
                    }
                    else
                    {//TODO find correct connection to send to

                        for (auto client : clients)
                        {
                            if (packet.body.find(client->UserName(), 6) != str.npos)
                            {
                                logger(LogLvl::Info) << packet.owner->UserName() << " " << str;
                                incManager >> outManager;
                                break;
                            }
                        }
                        logger(LogLvl::Warning) << "Could not find receiver -- deleting packet";
                        incManager.PopBack();
                        break;
                    }
                    break;
                case PacketType::Command:
                    //TODO implement commands for server
                    logger(LogLvl::Warning) << "Invalid command! Deleting!";
                    break;
                default:
                    if (packet.owner == nullptr)
                    {
                        logger(LogLvl::Warning) << "Unknown PackeTtype! deleting!";
                        incManager.PopBack();
                    }
                    else
                        incManager >> outManager;
                    break;
                }
                logger(LogLvl::Info) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';

            
            
            }
            return true;
        }

        int TcpServer::Start()
        {
            logger(LogLvl::Info) << "Starting Server";
            if (m_running != true) Stop();

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
                while (OnUserUpdate(incManager, outManager, connections) && m_running) {}
                m_running = false;                
            }
            return 0;
        }
        bool TcpServer::onClientDisconnect()
        {
            return 0;
        }
        void TcpServer::Stop()
        {   
            while (settings.networkThread) {};            
            // close sockets
            for (auto connection : connections)
                connection->Close();

           Network::ShutDownWinsock();
        }
        bool TcpServer::onClientConnect()
        {
            return 0;
        }
        void TcpServer::NetworkFrame()
        {
            int result = 0;
            int poll = 0;
            std::vector<WSAPOLLFD> useFDS = pollFDS;
            
            poll = WSAPoll(useFDS.data(), useFDS.size(), settings.timeout);
            if (poll > 0)
            {
                for (auto i = 0; i < useFDS.size(); i++)
                {
                    switch (useFDS[i].revents)
                    {
                    case 0:
                        continue;
                        
                    case  POLLRDNORM:
                        if (connections[i]->GetType() == Connection::Type::Listener)
                        {
                            std::shared_ptr<Connection> newConnection = connections[i]->Accept();
                            pollFDS.emplace_back(newConnection->PollFD());
                            connections.push_back(newConnection);
                            break;
                        }
                        else
                        {
                            int r = connections[i]->RecvAll();
                            switch (r)
                            {
                            case -1:
                                CloseConnection(i, "Recv < 0");
                                break;
                            case 0:
                                CloseConnection(i, "Recv = 0");
                                break;
                            default:
                                break;
                            }

                            break;
                        }
                        break;
                    case POLLERR:
                        CloseConnection(i, "POLLERR");

                        break;
                    case POLLHUP:
                        //TODO start timer for removing connection
                        CloseConnection(i, "POLLHUP");
                        break;
                    
                    case POLLNVAL:
                        CloseConnection(i, "POLLINVAL");
                        break;
                        
                    default:
                        logger(LogLvl::Warning) << "Revents: "<< useFDS[i].revents <<" On connectionID: " << connections[i];
                        break;
                    }
                }
            }
            if (poll == SOCKET_ERROR) logger(LogLvl::Error) << "PollErr: " << WSAGetLastError();
            

            while (outManager.Queue.size() > 0)
            {
                logger(LogLvl::Debug) << "Incomming PacketQue items : [" << incManager.Size() << "] Outgoing PacketQue items: [" << outManager.Size() << ']';
                Packet packet = outManager.PopBack();                
                if (packet.owner->SendAll(packet) != 0)
                    incManager << packet; //putting packet back in que if failed to send
            }
            if (!settings.networkThread && !settings.loop) m_running = OnUserUpdate(incManager, outManager, connections);
        }
        int TcpServer::Console()
        {            
            char buf[256]{};

            std::cin.getline(buf, sizeof(buf));
            std::string strBuf = buf;

            if (strBuf == "exit")
            {
                m_running = false;
                return -1;
            }

            std::vector<std::string> v = Helpers::Split(strBuf, ' ');
            if (v.size() > 2)
            {
                for (auto& client : connections)
                    if (client->UserName() == v[1] || v[1] == "all" || std::to_string(client->ID())==v[1])
                    {
                        try
                        {
                            if (client->GetType() != Connection::Type::Listener)
                            {
                                Packet packet(client->shared_from_this());
                                packet.header.type = (PacketType)stoi(v[0]);
                                v.erase(v.begin());
                                v[0] = "[Server]";
                                std::string msg= Helpers::Join(v, ' ');
                                packet << msg;
                                outManager << packet;
                            }
                        }
                        catch (const std::exception& err)
                        {
                            logger(LogLvl::Error) << err.what() << " Correct syntax: username intPackettype message/instruction ";
                        }
                    }
            }
            return 0;
        }
        int TcpServer::CloseConnection(int index, std::string reason)
        {
            connections[index]->Disconnect();            
            connections.erase(connections.begin() + index);
            pollFDS.erase(pollFDS.begin() + index);
            logger(LogLvl::Info) << "Removing Connection [" << index << "] " << reason;

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

