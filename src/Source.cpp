#include "CHIP8.h"

int main() {
	CHIP8 test;
	test.load_memory("assets/Space Invaders [David Winter] (alt).ch8");
	test.execute();
}