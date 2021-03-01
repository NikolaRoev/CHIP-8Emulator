#pragma once
#include "spdlog/spdlog.h"

#include <memory>

//======================================================================================================================================================

class Log {
private:
	static spdlog::logger* logger;

public:
	static void init();

	inline static spdlog::logger* get_logger() { return logger; }
};

//======================================================================================================================================================

#ifdef _DEBUG
	#define CHIP8_LOG_INIT()	::Log::init()
	#define CHIP8_TRACE(...)    SPDLOG_LOGGER_TRACE		(::Log::get_logger(), __VA_ARGS__)
	#define CHIP8_INFO(...)     SPDLOG_LOGGER_INFO		(::Log::get_logger(), __VA_ARGS__)
	#define CHIP8_WARN(...)     SPDLOG_LOGGER_WARN		(::Log::get_logger(), __VA_ARGS__)
	#define CHIP8_ERROR(...)    SPDLOG_LOGGER_ERROR		(::Log::get_logger(), __VA_ARGS__)
	#define CHIP8_CRITICAL(...) SPDLOG_LOGGER_CRITICAL	(::Log::get_logger(), __VA_ARGS__)
#else
	#define CHIP8_LOG_INIT()
	#define CHIP8_TRACE(...)
	#define CHIP8_INFO(...)
	#define CHIP8_WARN(...)
	#define CHIP8_ERROR(...)
	#define CHIP8_CRITICAL(...)
#endif

//======================================================================================================================================================