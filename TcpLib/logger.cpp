#include "logger.h"
// Setting default settings
	LogLvl logger::LogLevel  = LogLvl::Info;
	bool logger::LogHeader = true;
	std::mutex logger::mtxLog;
