#include "CHIP8.h"

int main() {
	CHIP8 test;
	test.load_memory("assets/br8kout.ch8");
	test.execute();
}