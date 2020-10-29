#pragma once
#include "Commands.h"
#include <vector>
#include <string>
#include "logger.h"



class cmdFactory
{
	
	Commands* ptr;
public:
	
	
	cmdFactory();
	
	~cmdFactory() 
	{ 
		logger(LogLvl::Info) << "CmdFactory Destructor";		
	}
	
	
	Commands* GetCmd(std::string cmd)
	{	
		for (size_t i = 0; i < cmdList.size(); i++)
		{			
			if (cmdList[i] == cmd)
			{	
				switch (i)
				{
				case 0:
					ptr = new cexit();
					return ptr;
				case 1:
					ptr = new cauth();
					return ptr;
				case 2:
					ptr = new ckey();
					return ptr;
				case 3:
					ptr = new cSetUserLvl();
					return ptr;
				case 4:
					ptr= new cTest();
					return ptr;				
				}
			}
			
		}
		Commands* ptr = new cUnknown();
		return ptr;
	}
};

