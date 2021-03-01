#include "Log/Log.h"
#include "CHIP8.h"

int main() {
	CHIP8_LOG_INIT();
	CHIP8_TRACE("Initiated log.");

	CHIP8 test;
	test.load_memory("roms/Space Invaders [David Winter] (alt).ch8");
	test.run();
}