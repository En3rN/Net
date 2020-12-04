#include "logger.h"
// Setting default settings
	LogLvl logger::LogLevel  = LogLvl::Debug;
	bool logger::LogHeader = true;
	std::mutex logger::mtxLog;
