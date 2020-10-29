//#include "HDServer.h"
//#include "Commands.h"
//#include <iostream>
//
//HDServer::HDServer(int port, bool console)
//{
//	
//}
//HDServer::~HDServer() {};
//
//void HDServer::Running(bool running)
//{
//	m_running = running;
//}
//
//int HDServer::runCommand(User& sender, std::vector<std::string>& vecCmd)
//{
//    Packet packet(GetSocket(sender));
//    if (vecCmd[0] == "exit")
//    {
//        if (sender.UserLevel >= UserLvl::Admin)
//        {
//            logger(LogLvl::Info) << "Shutting down server!";
//            m_running = false;
//            return 0;
//        }
//        else
//        {
//            logger(LogLvl::Warning) << sender.Name + " " << (int)sender.UserLevel << " : ACCESS DENIED : " << vecCmd[0];
//            return 0;
//        }
//    }
//    if (vecCmd[0] == "auth")
//    {
//        int parameters = 4;
//        if (vecCmd.size() == parameters)
//        {
//            //parameters auth name key tlf           
//            if (vecCmd[2] == "n/a")
//            {
//                sender.Name == vecCmd[1];
//                sender.Telephone == vecCmd[3];
//
//                //TODO: Request --> new user
//        //CreateTask(lvlPwrUser, sender.Name,Requesting KEY)     
//
//                logger(LogLvl::Info) << sender.Name << " Requesting KEY";
//                return 1;
//            }
//            //find all users registered
//            std::vector<User> stored = User::GetUsers();
//            for (size_t i = 0; i < stored.size(); i++)
//            {
//                if (stored[i].Name == vecCmd[1])
//                {
//                    if (stored[i].Key == vecCmd[2])
//                    {
//                        logger(LogLvl::Info) << sender.Name << " has AUTHED as " << stored[i].Name;
//                        sender = stored[i];
//                        return 0;
//                    }
//                    else
//                    {
//                        logger(LogLvl::Warning) << sender.Name << " ACCESS DENIED : wrong key!";
//
//                        //TODO: msg admin request new key?
//                        //CreateTask(lvlPwrUser, sender.Name, Wrong KEY!)
//                        return 2;
//                    }
//                }
//            }
//        }
//
//        logger(LogLvl::Info) << sender.Name << " Wrong number of parameters(" << vecCmd.size() << "/" << parameters;
//        return 3;
//    }
//
//    if (vecCmd[0] == "test")
//    {
//        for (auto& u : ConnectedUsers)
//        {            
//            Packet packet(GetSocket(sender));
//            packet.socket = GetSocket(u);
//            std::string msg = "Test sendt from: " + sender.Name;
//            packet << msg;
//
//            if (packet.socket != m_socket && packet.socket != 0)
//                outManager << packet;
//        }
//        return 0;
//    }
//
//    if (vecCmd[0] == "key")
//    {
//        if (sender.UserLevel < UserLvl::PwrUser)
//        {
//            logger(LogLvl::Warning) << sender.Name << " ACCESS DENIED : Need to be PowerUser to generate KEY!";
//            return 1;
//        }
//        if (vecCmd.size() >= 3)
//        {
//            std::vector<User> Users = User::GetUsers();
//            bool NewUser = true;
//            for (u_int i = 0; i < Users.size(); i++)
//                if (Users[i].Name == vecCmd[1])
//                {
//                    NewUser = false;
//                    Users[i].Key = User::GenerateKey();
//                    if (User::SaveUsers(Users) != 0)
//                        logger(LogLvl::Error) << "Error saving users!";
//                    else
//                        logger(LogLvl::Info) << "Users was saved!";
//                };
//            if (NewUser) Users.push_back(User::NewUser(vecCmd[1], (UserLvl)stoi(vecCmd[2])));
//
//            logger(LogLvl::Info) << sender.Name << " Created Key for: " << vecCmd[1];
//
//            for (size_t i = 0; i < ConnectedUsers.size(); i++)
//            {
//                if (vecCmd[1] == ConnectedUsers[i].Name && ConnectedUsers[i].UserLevel == UserLvl::NotAuthed)
//                {
//                    for (auto u : Users)
//                    {
//                        if (u.Name == vecCmd[1])
//                        {
//                            std::string response = "key " + u.Name + " " + u.Key;
//                            packet << response;
//                            packet.socket= masterFDS.fd_array[i];
//                            outManager << packet;
//                            return 0;
//                        }
//                    }
//                }
//            }
//            return 0;
//        }
//        logger(LogLvl::Info) << sender.Name << " Not enough Parameters: " << vecCmd[0];
//        return 1;
//    }
//    if (vecCmd[0] == "getoOnlineUsers")
//    {
//        std::string response;
//        for (auto u : ConnectedUsers)
//        {
//            response += u.Name + " ";
//        }
//        packet << response;
//        outManager << packet;
//    }
//    logger(LogLvl::Info) << "Invalid Command: " << vecCmd[0] << " from " << sender.Name;
//    return 1;
//}
//
//bool HDServer::OnUserCreate() 
//{
//	return true;
//}
//bool HDServer::OnUserUpdate(tsQueue<Packet>& incManager, tsQueue<Packet>& outManager, const std::vector<User>& ConnectedUsers)
//{
//	
//    if (m_settings.console)
//    {
//        while (m_running)
//        {
//            char buf[256];
//            ZeroMemory(buf, sizeof(buf));
//            std::cin.getline(buf, sizeof(buf));
//            std::string strBuf = buf;
//            if (strBuf == "exit")
//                return false;
//            //Packet packet;
//            //float outint = 500.3f;
//            //packet << strBuf << outint;
//            //incManager << packet;
//            //logger(LogLvl::Debug) << "Size: " << packet.Size() << " INT: " << outint <<" Command:" << strBuf;
//        }
//
//    }    
//    return true;
//}
//
//
//
//
//
////switch (m_settings.mode)
////{
////case AppMode::Server_Loop_Networkthread:
////    m_settings.server = true;
////    m_settings.timeout = { 2,0 };		    //NetworkFrame timeout on select() {0,0} is blocking    
////    m_settings.serverLoop = true;			//will loop OnUserUpdate()
////    m_settings.networkThread = true;		//will loop NetworkFrame() on a different thread
////    m_settings.console = true;              //blocking cin.get() in default OnUserUpdate()
//
////    break;
////case AppMode::Server_Loop:
////    m_settings.server = true;
////    m_settings.timeout = { 0,1 };		    //NetworkFrame timeout on select() {0,0} is blocking    
////    m_settings.serverLoop = true;			//will loop OnUserUpdate()
////    m_settings.networkThread = false;		//will loop NetworkFrame() on a different thread
////    m_settings.console = true;             //blocking cin.get() in default OnUserUpdate()
//
////    break;
////case AppMode::Server_NetworkFrame:
////    m_settings.server = true;
////    m_settings.timeout = { 0,1 };		    //NetworkFrame timeout on select() {0,0} is blocking    
////    m_settings.serverLoop = false;			//will loop OnUserUpdate()
////    m_settings.networkThread = false;		//will loop NetworkFrame() on a different thread
////    m_settings.console = false;             //blocking cin.get() in default OnUserUpdate()
//
////    break;
////case AppMode::Server_NetworkThread:
////    m_settings.server = true;
////    m_settings.timeout = { 2,0 };		    //NetworkFrame timeout on select() {0,0} is blocking    
////    m_settings.serverLoop = false;			//will loop OnUserUpdate()
////    m_settings.networkThread = true;		//will loop NetworkFrame() on a different thread
////    m_settings.console = true;              //blocking cin.get() in default OnUserUpdate()
//
////    break;
////case AppMode::Client_Loop_NetworkThread:
////    m_settings.server = false;
////    m_settings.timeout = { 2,0 };		    //NetworkFrame timeout on select() {0,0} is blocking    
////    m_settings.serverLoop = true;			//will loop OnUserUpdate()
////    m_settings.networkThread = true;		//will loop NetworkFrame() on a different thread
////    m_settings.console = true;              //blocking cin.get() in default OnUserUpdate()
//
////    break;
////case AppMode::Client_Loop:
////    m_settings.server = false;
////    m_settings.timeout = { 0,1 };		    //NetworkFrame timeout on select() {0,0} is blocking    
////    m_settings.serverLoop = true;			//will loop OnUserUpdate()
////    m_settings.networkThread = false;		//will loop NetworkFrame() on a different thread
////    m_settings.console = false;             //blocking cin.get() in default OnUserUpdate()        
////    break;
////case AppMode::Client_NetworkFrame:
////    m_settings.server = false;
////    m_settings.timeout = { 0,1 };		    //NetworkFrame timeout on select() {0,0} is blocking    
////    m_settings.serverLoop = false;			//will loop OnUserUpdate()
////    m_settings.networkThread = false;		//will loop NetworkFrame() on a different thread
////    m_settings.console = false;             //blocking cin.get() in default OnUserUpdate()
////    m_settings.server = true;
////    break;
////case AppMode::Client_NetworkThread:
////    m_settings.server = false;
////    m_settings.timeout = { 2,0 };		    //NetworkFrame timeout on select() {0,0} is blocking    
////    m_settings.serverLoop = false;			//will loop OnUserUpdate()
////    m_settings.networkThread = true;		//will loop NetworkFrame() on a different thread
////    m_settings.console = true;              //blocking cin.get() in default OnUserUpdate()
////    m_settings.server = true;
////    break;
////default:
////    logger(LogLvl::Error) << "Unknown Appmode, shutting down!!";
////    Stop();
////    break;
///*}
//switch (m_settings.ipSettings)
//{
//case IPSettings::IPv4:
//    break;
//case IPSettings::IPv4Restrict:
//    if (m_settings.ipAddress == "0,0,0,0")
//        logger(LogLvl::Warning) << "No ip specified! Using default";
//    break;
//case IPSettings::IPv6:
//    logger(LogLvl::Error) << "Not yet implemented!";
//    break;
//case IPSettings::IPv6Restrict:
//    logger(LogLvl::Error) << "Not yet implemented!";
//    break;
//default:
//    logger(LogLvl::Error) << "Unknown ipsettings, shutting down!!";
//    Stop();
//    break;
//}*/
////StartupWinsock(wsaData);
////m_socket = CreateSocket();
//////masterFDS << m_socket;
////initFDS(masterFDS, m_socket);
//
