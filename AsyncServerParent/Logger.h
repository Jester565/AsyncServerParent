/*
All error, debug, and print lines go through this class instead of directly to cout
so you can determine what log level you would like to output
*/

#pragma once
#include <iostream>
#include <string>
#include <stdio.h>

//All possible logLevels, the higher the number the more important
enum LOG_LEVEL {
	DebugLow = 0,
	DebugHigh = 1,
	Warning = 2,
	Error = 3
};

#define MAX_LOG_PRINTF_BUFFER_SIZE 100

//Allows you to use printf while it still going through the logger
#define LOG_PRINTF(level, msg, ...) \
{ \
	char buffer[MAX_LOG_PRINTF_BUFFER_SIZE];\
	snprintf(buffer, MAX_LOG_PRINTF_BUFFER_SIZE, msg, ##__VA_ARGS__);\
	Logger::Log(level, std::string(buffer));\
}

namespace Logger {
	//Determines which logLevels are printed, anything greater than or equal to this log level is printed
	static LOG_LEVEL logLevel = DebugLow;

	//Setter for logLevel
	static void SetLogLevel(LOG_LEVEL level) {
		logLevel = level;
	}

	//Provide the LOG_LEVEL of the message and then the message
	static void Log(LOG_LEVEL level, const std::string& str) {
		if (level >= logLevel) {
			std::cout << str << std::endl;
		}
	}
};
