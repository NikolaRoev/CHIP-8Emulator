#include "CHIP8.h"

#include <cstdint>

#include <array>

#include <fstream>
#include <ios>
#include <iostream>

#include <random>
#include <chrono>
#include <thread>


#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

//======================================================================================================================================================

void CHIP8::code_00E0() {
	screen = {};

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_00EE() {
	program_counter = stack[static_cast<uint64_t>(stack_pointer) - 1];
	stack_pointer -= 1;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_1nnn() {
	uint16_t hold = memory[program_counter] & 0b0000'1111;
	program_counter = (hold << 8) | memory[static_cast<uint64_t>(program_counter) + 1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_2nnn() {
	stack[stack_pointer] = program_counter;
	stack_pointer += 1;

	uint16_t hold = memory[program_counter] & 0b0000'1111;
	program_counter = (hold << 8) | memory[static_cast<uint64_t>(program_counter) + 1];
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
	uint16_t hold = memory[program_counter] & 0b0000'1111;
	address_register = (hold << 8) | memory[static_cast<uint64_t>(program_counter) + 1];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Bnnn() {
	uint16_t hold = memory[program_counter] & 0b0000'1111;
	program_counter = ((hold << 8) | memory[static_cast<uint64_t>(program_counter) + 1]) + registers[0x0ui8];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Cxkk() {
	registers[memory[program_counter] & 0b0000'1111] = static_cast<uint8_t>(rand() % 256) & memory[static_cast<uint64_t>(program_counter) + 1];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Dxyn() {
	uint8_t x = registers[memory[program_counter] & 0b0000'1111];
	uint8_t y = registers[memory[static_cast<uint64_t>(program_counter) + 1] >> 4];
	uint8_t n = memory[static_cast<uint64_t>(program_counter) + 1] & 0b0000'1111;

	
	for (uint8_t j = 0; j < n; ++j) {
		uint8_t byte_to_draw = memory[static_cast<uint64_t>(address_register) + j];

		for (uint8_t i = 0; i < 8; ++i) {
			uint8_t bit_to_draw = (byte_to_draw >> (7 - i)) & 1;

			if (bit_to_draw != 0 && screen[(j + y) % 32][(i + x) % 64] != 0) {
				registers[0xFui8] = 1;
			}
			else {
				registers[0xFui8] = 0;
			}

			screen[(j + y) % 32][(i + x) % 64] ^= static_cast<bool>(bit_to_draw);
		}
	}

	program_counter += 2;


	//Rendering:
	sf::RectangleShape set_pixel;
	set_pixel.setSize({ 10, 10 });
	set_pixel.setFillColor(sf::Color::Blue);

	sf::RectangleShape unset_pixel;
	unset_pixel.setSize({ 10, 10 });
	unset_pixel.setFillColor(sf::Color::Yellow);

	for (int screen_y = 0; screen_y < screen.size(); ++screen_y) {
		for (int screen_x = 0; screen_x < screen[0].size(); ++screen_x) {
			if (screen[screen_y][screen_x]) {
				set_pixel.setPosition(screen_x*10, screen_y*10);
				window.draw(set_pixel);
			}
			else {
				unset_pixel.setPosition(screen_x * 10, screen_y * 10);
				window.draw(unset_pixel);
			}
		}
	}

	//TO DO: Fix flipping and collision.
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Ex9E() {
	sf::Keyboard::Key key{};
	switch (registers[memory[program_counter] & 0b0000'1111]) {
		case 0x0ui8:
			key = sf::Keyboard::Num0;
			break;
		case 0x1ui8:
			key = sf::Keyboard::Num1;
			break;
		case 0x2ui8:
			key = sf::Keyboard::Num2;
			break;
		case 0x3ui8:
			key = sf::Keyboard::Num3;
			break;
		case 0x4ui8:
			key = sf::Keyboard::Num4;
			break;
		case 0x5ui8:
			key = sf::Keyboard::Num5;
			break;
		case 0x6ui8:
			key = sf::Keyboard::Num6;
			break;
		case 0x7ui8:
			key = sf::Keyboard::Num7;
			break;
		case 0x8ui8:
			key = sf::Keyboard::Num8;
			break;
		case 0x9ui8:
			key = sf::Keyboard::Num9;
			break;
		case 0xAui8:
			key = sf::Keyboard::A;
			break;
		case 0xBui8:
			key = sf::Keyboard::B;
			break;
		case 0xCui8:
			key = sf::Keyboard::C;
			break;
		case 0xDui8:
			key = sf::Keyboard::D;
			break;
		case 0xEui8:
			key = sf::Keyboard::E;
			break;
		case 0xFui8:
			key = sf::Keyboard::F;
			break;
	}


	if (sf::Keyboard::isKeyPressed(key)) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_ExA1() {
	sf::Keyboard::Key key{};
	switch (registers[memory[program_counter] & 0b0000'1111]) {
		case 0x0ui8:
			key = sf::Keyboard::Num0;
			break;
		case 0x1ui8:
			key = sf::Keyboard::Num1;
			break;
		case 0x2ui8:
			key = sf::Keyboard::Num2;
			break;
		case 0x3ui8:
			key = sf::Keyboard::Num3;
			break;
		case 0x4ui8:
			key = sf::Keyboard::Num4;
			break;
		case 0x5ui8:
			key = sf::Keyboard::Num5;
			break;
		case 0x6ui8:
			key = sf::Keyboard::Num6;
			break;
		case 0x7ui8:
			key = sf::Keyboard::Num7;
			break;
		case 0x8ui8:
			key = sf::Keyboard::Num8;
			break;
		case 0x9ui8:
			key = sf::Keyboard::Num9;
			break;
		case 0xAui8:
			key = sf::Keyboard::A;
			break;
		case 0xBui8:
			key = sf::Keyboard::B;
			break;
		case 0xCui8:
			key = sf::Keyboard::C;
			break;
		case 0xDui8:
			key = sf::Keyboard::D;
			break;
		case 0xEui8:
			key = sf::Keyboard::E;
			break;
		case 0xFui8:
			key = sf::Keyboard::F;
			break;
	}


	if (!sf::Keyboard::isKeyPressed(key)) {
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
	window.waitEvent(event);


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)) {
		registers[memory[program_counter] & 0b0000'1111] = 0x0ui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
		registers[memory[program_counter] & 0b0000'1111] = 0x1ui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
		registers[memory[program_counter] & 0b0000'1111] = 0x2ui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
		registers[memory[program_counter] & 0b0000'1111] = 0x3ui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
		registers[memory[program_counter] & 0b0000'1111] = 0x4ui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
		registers[memory[program_counter] & 0b0000'1111] = 0x5ui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6)) {
		registers[memory[program_counter] & 0b0000'1111] = 0x6ui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7)) {
		registers[memory[program_counter] & 0b0000'1111] = 0x7ui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8)) {
		registers[memory[program_counter] & 0b0000'1111] = 0x8ui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9)) {
		registers[memory[program_counter] & 0b0000'1111] = 0x9ui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		registers[memory[program_counter] & 0b0000'1111] = 0xAui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
		registers[memory[program_counter] & 0b0000'1111] = 0xBui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
		registers[memory[program_counter] & 0b0000'1111] = 0xCui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		registers[memory[program_counter] & 0b0000'1111] = 0xDui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
		registers[memory[program_counter] & 0b0000'1111] = 0xEui8;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
		registers[memory[program_counter] & 0b0000'1111] = 0xFui8;
	}


	program_counter += 2;
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
	address_register = registers[memory[program_counter] & 0b0000'1111] * 5;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx33() {
	memory[address_register] = registers[memory[program_counter] & 0b0000'1111] / 100;
	memory[static_cast<uint64_t>(address_register) + 1] = (registers[memory[program_counter] & 0b0000'1111] % 100) / 10;
	memory[static_cast<uint64_t>(address_register) + 2] = (registers[memory[program_counter] & 0b0000'1111] % 100) % 10;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx55() {
	for (uint8_t i = 0; i <= (memory[program_counter] & 0b0000'1111); ++i) {
		memory[static_cast<uint64_t>(address_register) + i] = registers[i];
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx65() {
	for (uint8_t i = 0; i <= (memory[program_counter] & 0b0000'1111); ++i) {
		registers[i] = memory[static_cast<uint64_t>(address_register) + i];
	}

	program_counter += 2;
}

//======================================================================================================================================================
//======================================================================================================================================================

bool CHIP8::load_memory(const char* file_name) {
	//Font data:
	//0.				 //1.				  //2.				    //3.				  //4.				    //5.				  //6.				    //7.
	memory[0] = 0xF0ui8; memory[5] = 0x20ui8; memory[10] = 0xF0ui8; memory[15] = 0xF0ui8; memory[20] = 0x90ui8; memory[25] = 0xF0ui8; memory[30] = 0xF0ui8; memory[35] = 0xF0ui8;
	memory[1] = 0x90ui8; memory[6] = 0x60ui8; memory[11] = 0x10ui8; memory[16] = 0x10ui8; memory[21] = 0x90ui8; memory[26] = 0x80ui8; memory[31] = 0x80ui8; memory[36] = 0x10ui8;
	memory[2] = 0x90ui8; memory[7] = 0x20ui8; memory[12] = 0xF0ui8; memory[17] = 0xF0ui8; memory[22] = 0xF0ui8; memory[27] = 0xF0ui8; memory[32] = 0xF0ui8; memory[37] = 0x20ui8;
	memory[3] = 0x90ui8; memory[8] = 0x20ui8; memory[13] = 0x80ui8; memory[18] = 0x10ui8; memory[23] = 0x10ui8; memory[28] = 0x10ui8; memory[33] = 0x90ui8; memory[38] = 0x40ui8;
	memory[4] = 0xF0ui8; memory[9] = 0x70ui8; memory[14] = 0xF0ui8; memory[19] = 0xF0ui8; memory[24] = 0x10ui8; memory[29] = 0xF0ui8; memory[34] = 0xF0ui8; memory[39] = 0x40ui8;

	//8.				  //9.				    //A.				  //B.				    //C.				  //D.				    //E.				  //F.
	memory[40] = 0xF0ui8; memory[45] = 0xF0ui8; memory[50] = 0xF0ui8; memory[55] = 0xE0ui8; memory[60] = 0xF0ui8; memory[65] = 0xE0ui8; memory[70] = 0xF0ui8; memory[75] = 0xF0ui8;
	memory[41] = 0x90ui8; memory[46] = 0x90ui8; memory[51] = 0x90ui8; memory[56] = 0x90ui8; memory[61] = 0x80ui8; memory[66] = 0x90ui8; memory[71] = 0x80ui8; memory[76] = 0x80ui8;
	memory[42] = 0xF0ui8; memory[47] = 0xF0ui8; memory[52] = 0xF0ui8; memory[57] = 0xE0ui8; memory[62] = 0x80ui8; memory[67] = 0x90ui8; memory[72] = 0xF0ui8; memory[77] = 0xF0ui8;
	memory[43] = 0x90ui8; memory[48] = 0x10ui8; memory[53] = 0x90ui8; memory[58] = 0x90ui8; memory[63] = 0x80ui8; memory[68] = 0x90ui8; memory[73] = 0x80ui8; memory[78] = 0x80ui8;
	memory[44] = 0xF0ui8; memory[49] = 0xF0ui8; memory[54] = 0x90ui8; memory[59] = 0xE0ui8; memory[64] = 0xF0ui8; memory[69] = 0xE0ui8; memory[74] = 0xF0ui8; memory[79] = 0x80ui8;


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

	window.create({ 640, 320 }, "CHIP-8 Emulator");

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
					std::cout << "Sound.\n";
				}
			}
		}
	);

	//------------------------------------------------------------------------------------------------------------------------------------------------------

	while (running) {
		//Clock delay:
		//std::this_thread::sleep_for(std::chrono::milliseconds(2));

		uint16_t instruction = (static_cast<uint16_t>(memory[program_counter]) << 8) | memory[static_cast<uint64_t>(program_counter) + 1];

		//------------------------------------------------------------------------------------------------------------------------------------------------------

		switch (memory[program_counter] >> 4) {
		case 0x0ui8:
			if (instruction == 0x00E0) {
				code_00E0();
			}
			else if (instruction == 0x00EE) {
				code_00EE();
			}
			else {
				running = false;
			}
			break;
		case 0x1ui8:
			code_1nnn(); break;
		case 0x2ui8:
			code_2nnn(); break;
		case 0x3ui8:
			code_3xkk(); break;
		case 0x4ui8:
			code_4xkk(); break;
		case 0x5ui8:
			code_5xy0(); break;
		case 0x6ui8:
			code_6xkk(); break;
		case 0x7ui8:
			code_7xkk(); break;
		case 0x8ui8:
			switch (instruction & 0b0000'0000'0000'1111) {
			case 0x0ui8:
				code_8xy0(); break;
			case 0x1ui8:
				code_8xy1(); break;
			case 0x2ui8:
				code_8xy2(); break;
			case 0x3ui8:
				code_8xy3(); break;
			case 0x4ui8:
				code_8xy4(); break;
			case 0x5ui8:
				code_8xy5(); break;
			case 0x6ui8:
				code_8xy6(); break;
			case 0x7ui8:
				code_8xy7(); break;
			case 0xEui8:
				code_8xyE(); break;
			default:
				running = false;
				break;
			}
			break;
		case 0x9ui8:
			code_9xy0(); break;
		case 0xAui8:
			code_Annn(); break;
		case 0xBui8:
			code_Bnnn(); break;
		case 0xCui8:
			code_Cxkk(); break;
		case 0xDui8:
			code_Dxyn(); break;
		case 0xEui8:
			if ((instruction & 0b0000'0000'1111'1111) == 0x9E) {
				code_Ex9E();
			}
			else if ((instruction & 0b0000'0000'1111'1111) == 0xA1) {
				code_ExA1();
			}
			else {
				running = false;
			}
			break;
		case 0xFui8:
			switch ((instruction & 0b0000'0000'1111'1111)) {
			case 0x07ui8:
				code_Fx07(); break;
			case 0x0Aui8:
				code_Fx0A(); break;
			case 0x15ui8:
				code_Fx15(); break;
			case 0x18ui8:
				code_Fx18(); break;
			case 0x1Eui8:
				code_Fx1E(); break;
			case 0x29ui8:
				code_Fx29(); break;
			case 0x33ui8:
				code_Fx33(); break;
			case 0x55ui8:
				code_Fx55(); break;
			case 0x65ui8:
				code_Fx65(); break;
			default:
				running = false;
				break;
			}
			break;
		default:
			running = false;
			break;
		}

		//------------------------------------------------------------------------------------------------------------------------------------------------------

		window.pollEvent(event);

		if (event.type == sf::Event::Closed) {
			window.close();
		}

		window.display();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------

	delay_thread.join();
	sound_thread.join();
}

//======================================================================================================================================================