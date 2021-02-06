#include "CHIP8.h"

int main() {
	CHIP8 test;
	test.load_memory("assets/snake.ch8");
	test.execute();
}