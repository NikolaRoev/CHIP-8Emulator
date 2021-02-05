#include "CHIP8.h"

#include "Instructions.h"

#include <Windows.h>

#include <cstdint>
#include <array>
#include <unordered_map>
#include <fstream>
#include <ios>
#include <random>
#include <chrono>
#include <thread>
#include <iostream>

//======================================================================================================================================================

void CHIP8::code_00E0() {
	screen = {};

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_00EE() {
	program_counter = stack[static_cast<uint64_t>(stack_pointer) - 1];
	stack_pointer -= 1;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_1nnn() {
	uint16_t hold = memory[program_counter];
	program_counter = (hold << 12) & memory[static_cast<uint64_t>(program_counter) + 1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_2nnn() {
	stack[stack_pointer] = program_counter;
	stack_pointer += 1;

	uint16_t hold = memory[program_counter];
	program_counter = (hold << 12) & memory[static_cast<uint64_t>(program_counter) + 1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_3xkk() {
	if (registers[memory[program_counter] & 0b0000'1111] == memory[static_cast<uint64_t>(program_counter) + 1]) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_4xkk() {
	if (registers[memory[program_counter] & 0b0000'1111] != memory[static_cast<uint64_t>(program_counter) + 1]) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_5xy0() {
	if (registers[memory[program_counter] & 0b0000'1111] == registers[memory[static_cast<uint64_t>(program_counter) + 1] >> 4]) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_6xkk() {
	registers[memory[program_counter] & 0b0000'1111] = memory[static_cast<uint64_t>(program_counter) + 1];
	
	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_7xkk() {
	registers[memory[program_counter] & 0b0000'1111] += memory[static_cast<uint64_t>(program_counter) + 1];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy0() {
	registers[memory[program_counter] & 0b0000'1111] = registers[memory[static_cast<uint64_t>(program_counter) + 1] >> 4];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy1() {
	registers[memory[program_counter] & 0b0000'1111] |= registers[memory[static_cast<uint64_t>(program_counter) + 1] >> 4];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy2() {
	registers[memory[program_counter] & 0b0000'1111] &= registers[memory[static_cast<uint64_t>(program_counter) + 1] >> 4];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy3() {
	registers[memory[program_counter] & 0b0000'1111] ^= registers[memory[static_cast<uint64_t>(program_counter) + 1] >> 4];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy4() {
	uint16_t temp_Vx = registers[memory[program_counter] & 0b0000'1111];
	uint16_t temp_Vy = registers[memory[static_cast<uint64_t>(program_counter) + 1] >> 4];

	temp_Vx += temp_Vy;

	registers[0xFui8] = (temp_Vx & 0b0000'0001'0000'0000) >> 8;

	temp_Vx &= 0b0000'0000'1111'1111;

	registers[memory[program_counter] & 0b0000'1111] = static_cast<uint8_t>(temp_Vx);

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy5() {
	if (registers[memory[program_counter] & 0b0000'1111] > registers[memory[static_cast<uint64_t>(program_counter) + 1] >> 4]) {
		registers[0xFui8] = 1;
	}
	else {
		registers[0xFui8] = 0;
	}

	registers[memory[program_counter] & 0b0000'1111] -= registers[memory[static_cast<uint64_t>(program_counter) + 1] >> 4];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy6() {
	registers[0xFui8] = registers[memory[program_counter] & 0b0000'1111] & 0b0000'0001;

	registers[memory[program_counter] & 0b0000'1111] >>= 1;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy7() {
	if (registers[memory[program_counter] & 0b0000'1111] < registers[memory[static_cast<uint64_t>(program_counter) + 1] >> 4]) {
		registers[0xFui8] = 1;
	}
	else {
		registers[0xFui8] = 0;
	}

	registers[memory[program_counter] & 0b0000'1111] = registers[memory[static_cast<uint64_t>(program_counter) + 1] >> 4] - registers[memory[program_counter] & 0b0000'1111];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xyE() {
	registers[0xFui8] = (registers[memory[program_counter] & 0b0000'1111] & 0b1000'0000) >> 7;

	registers[memory[program_counter] & 0b0000'1111] <<= 1;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_9xy0() {
	if (registers[memory[program_counter] & 0b0000'1111] != registers[memory[static_cast<uint64_t>(program_counter) + 1] >> 4]) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Annn() {
	uint16_t hold = memory[program_counter];
	address_register = (hold << 12) & memory[static_cast<uint64_t>(program_counter) + 1];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Bnnn() {
	uint16_t hold = memory[program_counter];
	program_counter = ((hold << 12) & memory[static_cast<uint64_t>(program_counter) + 1]) + registers[0x0ui8];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Cxkk() {
	registers[memory[program_counter] & 0b0000'1111] = static_cast<uint8_t>(rand() % 256) & memory[static_cast<uint64_t>(program_counter) + 1];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Dxyn() {
	//Sprite draw.
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Ex9E() {
	int key_code = 0;
	switch (registers[memory[program_counter] & 0b0000'1111]) {
		case 0x0ui8:
			key_code = '0';
			break;
		case 0x1ui8:
			key_code = '1';
			break;
		case 0x2ui8:
			key_code = '2';
			break;
		case 0x3ui8:
			key_code = '3';
			break;
		case 0x4ui8:
			key_code = '4';
			break;
		case 0x5ui8:
			key_code = '5';
			break;
		case 0x6ui8:
			key_code = '6';
			break;
		case 0x7ui8:
			key_code = '7';
			break;
		case 0x8ui8:
			key_code = '8';
			break;
		case 0x9ui8:
			key_code = '9';
			break;
		case 0xAui8:
			key_code = 'A';
			break;
		case 0xBui8:
			key_code = 'B';
			break;
		case 0xCui8:
			key_code = 'C';
			break;
		case 0xDui8:
			key_code = 'D';
			break;
		case 0xEui8:
			key_code = 'E';
			break;
		case 0xFui8:
			key_code = 'F';
			break;
	}


	//SHORT is 16 bits.
	if (GetKeyState(key_code) & 0b1000'0000'0000'0000) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_ExA1() {
	int key_code = 0;
	switch (registers[memory[program_counter] & 0b0000'1111]) {
		case 0x0ui8:
			key_code = '0';
			break;
		case 0x1ui8:
			key_code = '1';
			break;
		case 0x2ui8:
			key_code = '2';
			break;
		case 0x3ui8:
			key_code = '3';
			break;
		case 0x4ui8:
			key_code = '4';
			break;
		case 0x5ui8:
			key_code = '5';
			break;
		case 0x6ui8:
			key_code = '6';
			break;
		case 0x7ui8:
			key_code = '7';
			break;
		case 0x8ui8:
			key_code = '8';
			break;
		case 0x9ui8:
			key_code = '9';
			break;
		case 0xAui8:
			key_code = 'A';
			break;
		case 0xBui8:
			key_code = 'B';
			break;
		case 0xCui8:
			key_code = 'C';
			break;
		case 0xDui8:
			key_code = 'D';
			break;
		case 0xEui8:
			key_code = 'E';
			break;
		case 0xFui8:
			key_code = 'F';
			break;
	}



	//SHORT is 16 bits.
	if (!(GetKeyState(key_code) & 0b1000'0000'0000'0000)) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx07() {
	registers[memory[program_counter] & 0b0000'1111] = delay_register;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx0A() {
	//Input wait.
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx15() {
	delay_register = registers[memory[program_counter] & 0b0000'1111];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx18() {
	sound_register = registers[memory[program_counter] & 0b0000'1111];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx1E() {
	address_register += registers[memory[program_counter] & 0b0000'1111];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx29() {
	//Hardcoded hex sprites.
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx33() {
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx55() {
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx65() {
}

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

	//------------------------------------------------------------------------------------------------------------------------------------------------------

	std::thread delay_thread(
		[&delay_register = delay_register, &running]() {
			while (running) {
				std::this_thread::sleep_for(std::chrono::milliseconds(17));

				if (delay_register > 0) {
					delay_register--;
				}
			}
		}
	);

	std::thread sound_thread(
		[&sound_register = sound_register, &running]() {
			while (running) {
				std::this_thread::sleep_for(std::chrono::milliseconds(17));

				if (sound_register > 0) {
					sound_register--;
				}
			}
		}
	);

	//------------------------------------------------------------------------------------------------------------------------------------------------------

	while (running) {
		//Clock delay:
		std::this_thread::sleep_for(std::chrono::milliseconds(2));


		uint16_t instruction = (static_cast<uint16_t>(memory[program_counter]) << 8) & memory[static_cast<uint64_t>(program_counter) + 1];

		if ((instruction & Instructions::CLS) == Instructions::CLS) {
			code_00E0();
		}
		else {
			running = false;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------

	delay_thread.join();
	sound_thread.join();
}

//======================================================================================================================================================