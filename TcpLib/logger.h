#pragma once
#include <iostream>
#include <sstream>
#include "User.h"
#include "consolecolor.h"
#include <mutex>


    enum class LogLvl { Debug, Info, Warning, Error, Msg };

    class logger
    {
        LogLvl          msgLevel = LogLvl::Info;        
        bool            m_opened = false;
        static std::mutex      mtxLog;


    public:
        class ScopedSettings 
        {
            friend logger;
            LogLvl  scopedLvl = LogLvl::Error;
            bool    scopedHeader = LogHeader;
            static bool scopedSettings;            

        public:
            ScopedSettings(LogLvl loglvl, bool header)
            {
                scopedLvl = loglvl;
                scopedHeader = LogHeader;                
                //Settings(loglvl, header);
                logger(LogLvl::Info) << "Setting loggersettings: "  << (int)(loglvl) << '/' << header;
            } 
            ~ScopedSettings()
            {
                //logger(LogLvl::Info) << "ReSetting loggersettings: " << (int)oldLvl << '/' << oldHeader;
                //Settings(oldLvl, oldHeader);
            }
        };
        static LogLvl   LogLevel;
        static bool     LogHeader;

        logger()
        {
            msgLevel = LogLvl::Info;
        };

        logger(LogLvl type)
        {
            mtxLog.lock();
            msgLevel = type;
            if (LogHeader && msgLevel >= LogLevel) PrintLabel(type);
        }
        
        ~logger()
        {
            if (m_opened) {
                std::cout << std::endl;
            }
            m_opened = false;
            mtxLog.unlock();
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
        
        static ScopedSettings Get(LogLvl lvl, bool header)
        {
            ScopedSettings ss(lvl, header);
            return ss;
        }

        static void Settings(LogLvl lvl, bool header)
        {
            LogLevel = lvl;
            LogHeader = header;
        }
    private:

        inline void PrintLabel(LogLvl type) {
            switch (type)
            {
            case LogLvl::Debug:     std::cout << blue << "<DEBUG>" << white; break;
            case LogLvl::Info:      std::cout << green << "<INFO>" << white; break;
            case LogLvl::Warning:   std::cout << yellow << "<WARNING>" << white; break;
            case LogLvl::Error:     std::cout << red << "<ERROR>" << white; break;
            }
        }
    };
