#pragma once
//ip, port, timeout, loop, netth, consoleth
struct AppSettings
{	
	const char* ip;
	int			port;	
	int			timeout;			//NetworkFrame timeout on select() {0,0} is blocking
	bool		loop;				//will loop OnUserUpdate()
	bool		networkThread;		//will loop NetworkFrame() on a different thread
	bool		consoleThread;		
};