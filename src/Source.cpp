#include "CHIP8.h"

#include <iostream>

int main() {
	CHIP8 test;
	test.load_memory("assets/octojam1title.ch8");
	test.execute();
}