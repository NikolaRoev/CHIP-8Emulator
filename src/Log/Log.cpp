#include "Log.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

#include <memory>

//======================================================================================================================================================

spdlog::logger* Log::logger;

//======================================================================================================================================================

void Log::init() {
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
	console_sink->set_pattern("%^[%T %s:%# %!]: %v%$");

	logger = new spdlog::logger("CHIP-8 Emulator Log", console_sink);
	logger->set_level(spdlog::level::trace);
}

//======================================================================================================================================================