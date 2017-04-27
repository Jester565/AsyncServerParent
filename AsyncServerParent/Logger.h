#pragma once
#include <iostream>
#include <string>
#include <stdio.h>

enum LOG_LEVEL {
	DebugLow = 0,
	DebugHigh = 1,
	Warning = 2,
	Error = 3
};

#define MAX_LOG_PRINTF_BUFFER_SIZE 100

#define LOG_PRINTF(level, msg, ...) \
{ \
	char buffer[MAX_LOG_PRINTF_BUFFER_SIZE];\
	snprintf(buffer, MAX_LOG_PRINTF_BUFFER_SIZE, msg, ##__VA_ARGS__);\
	Logger::Log(level, std::string(buffer));\
}


namespace Logger {
	static LOG_LEVEL logLevel = DebugLow;

	static void SetLogLevel(LOG_LEVEL level) {
		logLevel = level;
	}

	static void Log(LOG_LEVEL level, const std::string& str) {
		if (level >= logLevel) {
			std::cout << str << std::endl;
		}
	}
};
