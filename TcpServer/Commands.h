//#pragma once
//#include <vector>
//#include "HDServer.h"
//
//class HDServer;
//class Commands
//{
//protected:
//	HDServer* serverPtr;
//	size_t parameters;
//	std::vector<std::string> cmdList = { "exit", "auth", "key", "setUserLvl", "test" };
//public:	
//	Commands(HDServer* server); 
//	Commands() {};
//	virtual ~Commands() {};
//	inline virtual int RunCmd(std::vector<std::string>& vCommand,
//						User& user,
//						SOCKET& socket);
//	Commands GetCmd(std::string cmd);
//};
//
//class cexit : public Commands
//{
//public:	
//	cexit() { parameters = 1; };
//	~cexit() { logger(LogLvl::Info) << "cmd obj deleted"; };
//
//	inline int RunCmd(std::vector<std::string>& vCommand,
//		User& user,
//		SOCKET& socket) override;
//};
//class cauth : public Commands
//{
//public:
//	cauth() { parameters = 4; };
//	~cauth() { logger(LogLvl::Info) << "cmd obj deleted"; };
//	inline int RunCmd(std::vector<std::string>& vCommand,
//		User& user,
//		SOCKET& socket) override;
//};
//
//class ckey : public Commands
//{
//public:	
//	ckey() { };
//	~ckey() { logger(LogLvl::Info) << "cmd obj deleted"; };
//	inline int RunCmd(std::vector<std::string>& vCommand,
//		User& user,
//		SOCKET& socket)override;	
//};
//class cSetUserLvl :public Commands
//{
//public:
//	cSetUserLvl () { };
//	~cSetUserLvl() { logger(LogLvl::Info) << "cmd obj deleted"; };
//	inline int RunCmd(std::vector<std::string>& vCommand,
//		User& user,
//		SOCKET& socket)override;
//	
//};
//class cTest :public Commands
//{
//public:
//	cTest() { };
//	~cTest() { logger(LogLvl::Info) << "cmd obj deleted"; };
//	
//	inline int RunCmd(std::vector<std::string>& vCommand,
//		User& user,
//		SOCKET& socket)override;
//	
//};
//class cUnknown : public Commands
//{
//public:
//	cUnknown() { };
//	~cUnknown() { logger(LogLvl::Info) << "cmd obj deleted"; };
//	inline int RunCmd(std::vector<std::string>& vCommand,
//		User& user,
//		SOCKET& socket)override;
//	
//};
