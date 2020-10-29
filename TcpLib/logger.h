#pragma once
#include <iostream>
#include <sstream>
#include "User.h"
#include "consolecolor.h"


enum class LogLvl { Debug, Info, Warning, Error, Msg };
 
class logger
{
    LogLvl  msgLevel = LogLvl::Debug;
    bool    m_opened = false;

public:
    static LogLvl   LogLevel;
    static bool     LogHeader;
    
    logger() 
    {
        msgLevel = LogLvl::Msg;
    };


    logger(LogLvl type)
    {   
        msgLevel = type;
        if (LogHeader && msgLevel >= LogLevel) PrintLabel(type);
    }
    ~logger() 
    {
        if (m_opened) {
            std::cout << std::endl;
        }
        m_opened = false;
    }
    template<typename t>
    logger& operator<<(const t& msg) 
    {
        if (msgLevel >= LogLevel) 
        {
            std::cout << msg;
            m_opened = true;
        }
        return *this;
    }
    static void settings(LogLvl lvl, bool Header)
    {
        LogLevel = lvl;
        LogHeader = Header;
    }
private:
 
    inline void PrintLabel(LogLvl type) {
        switch (type) 
        {
            case LogLvl::Debug:     std::cout   << blue     << "<DEBUG>"    << white; break;
            case LogLvl::Info:      std::cout   << green    << "<INFO>"     << white; break;
            case LogLvl::Warning:   std::cout   << yellow   << "<WARNING>"  << white; break;
            case LogLvl::Error:     std::cout   << red      << "<ERROR>"    << white; break;
        }        
    }
};

//"\033[31m"