//#include <vector>
//#include <string>
//#include "HDServer.h"
//#include "Commands.h"
//#include "User.h"
//#include "logger.h"
//
//Commands::Commands(HDServer* server)
//{
//	{serverPtr = server; };
//}
//
//inline int Commands::RunCmd(std::vector<std::string>& vCommand,
//	User& user,
//	SOCKET& socket)
//{
//	
//	return 0;
//};
//Commands Commands::GetCmd(std::string cmd)
//{
//	Commands ptr;
//	for (size_t i = 0; i < cmdList.size(); i++)
//	{
//		if (cmdList[i] == cmd)
//		{
//			switch (i)
//			{
//			case 0:
//				ptr =  cexit();
//				return ptr;
//			case 1:
//				ptr =  cauth();
//				return ptr;
//			case 2:
//				ptr =  ckey();
//				return ptr;
//			case 3:
//				ptr =  cSetUserLvl();
//				return ptr;
//			case 4:
//				ptr =  cTest();
//				return ptr;
//			}
//		}
//
//	}
//	ptr =  cUnknown();
//	return ptr;
//}
//
//inline int cexit::RunCmd(std::vector<std::string>& vCommand,
//					User& user,
//					SOCKET& socket) 
//{
//
//	if (user.UserLevel >= UserLvl::Admin)
//	{
//		logger(LogLvl::Info) << "Shutting down server!";
//
//		serverPtr->Running(false);
//		return 0;
//	}
//	else
//	{
//		logger(LogLvl::Warning) << user.Name + " " << (int)user.UserLevel << " : ACCESS DENIED : " << vCommand[0];
//		return -1;
//	}
//}
//
//inline int cauth::RunCmd(std::vector<std::string>& vCommand,
//					User& user,
//					SOCKET& socket) 
//{	
//	if (vCommand.size() == parameters)
//	{
//		//parameters auth name key tlf           
//		if (vCommand[2] == "n/a")
//		{
//			user.Name = vCommand[1];
//			user.Telephone = vCommand[3];
//
//			logger(LogLvl::Info) << user.Name << " Requesting KEY";
//			return 1;
//		}
//		//find all users registered
//		std::vector<User> stored = User::GetUsers();
//		for (size_t i = 0; i < stored.size(); i++)
//		{
//			if (stored[i].Name == vCommand[1])
//			{
//				if (stored[i].Key == vCommand[2])
//				{
//					logger(LogLvl::Info) << user.Name << " has AUTHED as " << stored[i].Name;
//					user = stored[i];
//					return 0;
//				}
//				else
//				{
//					logger(LogLvl::Warning) << user.Name << " ACCESS DENIED : wrong key!";
//
//					//TODO: msg admin request new key?
//					//CreateTask(lvlPwrUser, sender.Name, Wrong KEY!)
//					return 2;
//				}
//			}
//		}
//	}
//	//TODO: Request --> new user
//	//CreateTask(lvlPwrUser, sender.Name,Requesting KEY)        
//	logger(LogLvl::Info) << user.Name << " Wrong number of parameters("<< vCommand.size() << "/"<<parameters;
//	return 3;	
//}
//
//inline int ckey::RunCmd(std::vector<std::string>& vCommand,
//	User& user,
//	SOCKET& socket)
//{
//	if (sender.UserLevel < UserLvl::PwrUser)
//	{
//		logger(LogLvl::Warning) << sender.Name << " ACCESS DENIED : Need to be PowerUser to generate KEY!";
//		return 1;
//	}
//	if (vecCmd.size() >= 3)
//	{
//		std::vector<User> Users = User::GetUsers();
//		bool NewUser = true;
//		for (u_int i = 0; i < Users.size(); i++)
//			if (Users[i].Name == vecCmd[1])
//			{
//				NewUser = false;
//				Users[i].Key = User::GenerateKey();
//				if (User::SaveUsers(Users) != 0)
//					logger(LogLvl::Error) << "Error saving users!";
//				else
//					logger(LogLvl::Info) << "Users was saved!";
//			};
//		if (NewUser) Users.push_back(User::NewUser(vecCmd[1], (UserLvl)stoi(vecCmd[2])));
//
//		logger(LogLvl::Info) << sender.Name << " Created Key for: " << vecCmd[1];
//
//		for (size_t i = 0; i < serverPtr->ConnectedUsers.size(); i++)
//		{
//			if (vecCmd[1] == ConnectedUsers[i].Name && ConnectedUsers[i].UserLevel == UserLvl::NotAuthed)
//			{
//				for (auto u : Users)
//				{
//					if (u.Name == vecCmd[1])
//					{
//						std::string strBuf = "Key " + u.Name + " " + u.Key;
//						return SendToClient(masterFDS.fd_array[i], strBuf);
//					}
//				}
//			}
//		}
//		return 0;
//	}
//	logger(LogLvl::Info) << sender.Name << " Not enough Parameters: " << vecCmd[0];
//	return 1;
//	
//}
//
//inline int cSetUserLvl::RunCmd(std::vector<std::string>& vCommand,
//	User& user,
//	SOCKET& socket)
//{
//
//	return 0;
//}
//
//inline int cTest::RunCmd(std::vector<std::string>& vCommand,
//	User& user,
//	SOCKET& socket)
//{
//	{
//		for (auto s : serverPtr->masterFDS.fd_array)
//			if (s != serverPtr->listenSocket)
//				serverPtr->SendToClient(s, "Test sendt from: " + user.Name);
//
//		return 0;
//	}
//
//	return 0;
//}
//
//int cUnknown::RunCmd(std::vector<std::string>& vCommand,
//	User& user,
//	SOCKET& socket)
//{
//
//	return 0;
//}