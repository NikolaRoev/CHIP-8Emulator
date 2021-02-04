#include "CHIP8.h"

#include <cstdint>

#include <array>
#include <unordered_map>

#include <fstream>
#include <ios>

#include <iostream>

//======================================================================================================================================================



//======================================================================================================================================================
//======================================================================================================================================================

bool CHIP8::load_memory(const char* file_name) {
	std::ifstream file(file_name, std::ios::in|std::ios::binary|std::ios::ate);

	if (file.is_open()) {
		std::streampos file_size = file.tellg();
		file.seekg(0, std::ios::beg);


		//Load memory from the 512th byte.
		file.read((char*)&memory[512], file_size);


		file.close();
		return true;
	}
	else {
		return false;
	}
}

//======================================================================================================================================================

void CHIP8::execute() {
	bool running = true;

	while (running) {
		//Get the first four bits.
		switch (memory[program_counter] >> 4) {
			case 0x0:
				//TO DO:
				break;
		}
	}
}

//======================================================================================================================================================