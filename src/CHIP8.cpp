#include "CHIP8.h"

#include "Log/Log.h"

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
	CHIP8_TRACE("Opcode: [00E0]");

	screen = {};

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_00EE() {
	CHIP8_TRACE("Opcode: [00EE]");

	stack_pointer--;
	program_counter = stack[stack_pointer];
	
	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_1nnn() {
	CHIP8_TRACE("Opcode: [1nnn]");

	program_counter = instruction.address;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_2nnn() {
	CHIP8_TRACE("Opcode: [2nnn]");

	stack[stack_pointer] = program_counter;
	stack_pointer++;

	program_counter = instruction.address;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_3xkk() {
	CHIP8_TRACE("Opcode: [3xkk]");

	if (registers[instruction.second_nibble] == instruction.low_byte) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_4xkk() {
	CHIP8_TRACE("Opcode: [4xkk]");

	if (registers[instruction.second_nibble] != instruction.low_byte) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_5xy0() {
	CHIP8_TRACE("Opcode: [5xy0]");

	if (registers[instruction.second_nibble] == registers[instruction.third_nibble]) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_6xkk() {
	CHIP8_TRACE("Opcode: [6xkk]");

	registers[instruction.second_nibble] = instruction.low_byte;
	
	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_7xkk() {
	CHIP8_TRACE("Opcode: [7xkk]");

	registers[instruction.second_nibble] += static_cast<uint8_t>(instruction.low_byte);

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy0() {
	CHIP8_TRACE("Opcode: [8xy0]");

	registers[instruction.second_nibble] = registers[instruction.third_nibble];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy1() {
	CHIP8_TRACE("Opcode: [8xy1]");

	registers[instruction.second_nibble] |= registers[instruction.third_nibble];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy2() {
	CHIP8_TRACE("Opcode: [8xy2]");

	registers[instruction.second_nibble] &= registers[instruction.third_nibble];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy3() {
	CHIP8_TRACE("Opcode: [8xy3]");

	registers[instruction.second_nibble] ^= registers[instruction.third_nibble];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy4() {
	CHIP8_TRACE("Opcode: [8xy4]");

	uint16_t temp_Vx = registers[instruction.second_nibble];
	uint16_t temp_Vy = registers[instruction.third_nibble];

	temp_Vx += temp_Vy;

	registers[0xF] = (temp_Vx & 0x100) == 0x100;

	temp_Vx &= 0xFF;

	registers[instruction.second_nibble] = static_cast<uint8_t>(temp_Vx);

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy5() {
	CHIP8_TRACE("Opcode: [8xy5]");

	if (registers[instruction.second_nibble] > registers[instruction.third_nibble]) {
		registers[0xF] = 1;
	}
	else {
		registers[0xF] = 0;
	}

	registers[instruction.second_nibble] -= registers[instruction.third_nibble];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy6() {
	CHIP8_TRACE("Opcode: [8xy6]");

	registers[0xF] = registers[instruction.second_nibble] & 0x01;

	registers[instruction.second_nibble] >>= 1;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy7() {
	CHIP8_TRACE("Opcode: [8xy7]");

	if (registers[instruction.second_nibble] < registers[instruction.third_nibble]) {
		registers[0x0F] = 1;
	}
	else {
		registers[0x0F] = 0;
	}

	registers[instruction.second_nibble] = registers[instruction.third_nibble] - registers[instruction.second_nibble];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xyE() {
	CHIP8_TRACE("Opcode: [8xyE]");

	registers[0xF] = (registers[instruction.second_nibble] & 0x80) == 0x80;

	registers[instruction.second_nibble] <<= 1;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_9xy0() {
	CHIP8_TRACE("Opcode: [9xy0]");

	if (registers[instruction.second_nibble] != registers[instruction.third_nibble]) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Annn() {
	CHIP8_TRACE("Opcode: [Annn]");

	address_register = instruction.address;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Bnnn() {
	CHIP8_TRACE("Opcode: [Bnnn]");

	program_counter = instruction.address + registers[0x0];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Cxkk() {
	CHIP8_TRACE("Opcode: [Cxkk]");

	registers[instruction.second_nibble] = static_cast<uint8_t>(rand() % 256) & instruction.low_byte;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Dxyn() {
	CHIP8_TRACE("Opcode: [Dxyn]");

	uint8_t x = registers[instruction.second_nibble];
	uint8_t y = registers[instruction.third_nibble];
	uint8_t n = instruction.fourth_nibble;

	uint8_t collision = 0;
	
	for (uint8_t j = 0; j < n; ++j) {
		uint8_t byte_to_draw = memory[static_cast<uint64_t>(address_register) + j];

		for (uint8_t i = 0; i < 8; ++i) {
			uint8_t bit_to_draw = (byte_to_draw >> (7 - i)) & 1;

			if (bit_to_draw != 0 && screen[(j + y) % 32][(i + x) % 64] != 0) {
				collision = 1;
			}

			screen[(j + y) % 32][(i + x) % 64] ^= static_cast<bool>(bit_to_draw);
		}
	}

	registers[0xF] = collision;

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
				set_pixel.setPosition(static_cast<float>(screen_x * 10), static_cast<float>(screen_y * 10));
				window.draw(set_pixel);
			}
			else {
				unset_pixel.setPosition(static_cast<float>(screen_x * 10), static_cast<float>(screen_y * 10));
				window.draw(unset_pixel);
			}
		}
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Ex9E() {
	CHIP8_TRACE("Opcode: [Ex9E]");

	sf::Keyboard::Key key{ sf::Keyboard::Key::Unknown };

	switch (registers[instruction.second_nibble]) {
		case 0x00:
			key = sf::Keyboard::Num0;
			break;
		case 0x01:
			key = sf::Keyboard::Num1;
			break;
		case 0x02:
			key = sf::Keyboard::Num2;
			break;
		case 0x03:
			key = sf::Keyboard::Num3;
			break;
		case 0x04:
			key = sf::Keyboard::Num4;
			break;
		case 0x05:
			key = sf::Keyboard::Num5;
			break;
		case 0x06:
			key = sf::Keyboard::Num6;
			break;
		case 0x07:
			key = sf::Keyboard::Num7;
			break;
		case 0x08:
			key = sf::Keyboard::Num8;
			break;
		case 0x09:
			key = sf::Keyboard::Num9;
			break;
		case 0x0A:
			key = sf::Keyboard::A;
			break;
		case 0x0B:
			key = sf::Keyboard::B;
			break;
		case 0x0C:
			key = sf::Keyboard::C;
			break;
		case 0x0D:
			key = sf::Keyboard::D;
			break;
		case 0x0E:
			key = sf::Keyboard::E;
			break;
		case 0x0F:
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
	CHIP8_TRACE("Opcode: [ExA1]");

	sf::Keyboard::Key key{ sf::Keyboard::Key::Unknown };

	switch (registers[instruction.second_nibble]) {
		case 0x00:
			key = sf::Keyboard::Num0;
			break;
		case 0x01:
			key = sf::Keyboard::Num1;
			break;
		case 0x02:
			key = sf::Keyboard::Num2;
			break;
		case 0x03:
			key = sf::Keyboard::Num3;
			break;
		case 0x04:
			key = sf::Keyboard::Num4;
			break;
		case 0x05:
			key = sf::Keyboard::Num5;
			break;
		case 0x06:
			key = sf::Keyboard::Num6;
			break;
		case 0x07:
			key = sf::Keyboard::Num7;
			break;
		case 0x08:
			key = sf::Keyboard::Num8;
			break;
		case 0x09:
			key = sf::Keyboard::Num9;
			break;
		case 0x0A:
			key = sf::Keyboard::A;
			break;
		case 0x0B:
			key = sf::Keyboard::B;
			break;
		case 0x0C:
			key = sf::Keyboard::C;
			break;
		case 0x0D:
			key = sf::Keyboard::D;
			break;
		case 0x0E:
			key = sf::Keyboard::E;
			break;
		case 0x0F:
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
	CHIP8_TRACE("Opcode: [Fx07]");

	registers[instruction.second_nibble] = delay_register;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx0A() {
	CHIP8_TRACE("Opcode: [Fx0A]");

	window.waitEvent(event);


	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0)) {
		registers[instruction.second_nibble] = 0x00;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1)) {
		registers[instruction.second_nibble] = 0x01;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) {
		registers[instruction.second_nibble] = 0x02;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3)) {
		registers[instruction.second_nibble] = 0x03;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num4)) {
		registers[instruction.second_nibble] = 0x04;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num5)) {
		registers[instruction.second_nibble] = 0x05;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num6)) {
		registers[instruction.second_nibble] = 0x06;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num7)) {
		registers[instruction.second_nibble] = 0x07;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num8)) {
		registers[instruction.second_nibble] = 0x08;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num9)) {
		registers[instruction.second_nibble] = 0x09;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		registers[instruction.second_nibble] = 0x0A;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::B)) {
		registers[instruction.second_nibble] = 0x0B;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
		registers[instruction.second_nibble] = 0x0C;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		registers[instruction.second_nibble] = 0x0D;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
		registers[instruction.second_nibble] = 0x0E;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
		registers[instruction.second_nibble] = 0x0F;
	}


	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx15() {
	CHIP8_TRACE("Opcode: [Fx15]");

	delay_register = registers[instruction.second_nibble];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx18() {
	CHIP8_TRACE("Opcode: [Fx18]");

	sound_register = registers[instruction.second_nibble];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx1E() {
	CHIP8_TRACE("Opcode: [Fx1E]");

	address_register += registers[instruction.second_nibble];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx29() {
	CHIP8_TRACE("Opcode: [Fx29]");

	address_register = (registers[instruction.second_nibble] * 5) + 0x50;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx33() {
	CHIP8_TRACE("Opcode: [Fx33]");

	memory[address_register] = registers[instruction.second_nibble] / 100;
	memory[static_cast<uint64_t>(address_register) + 1] = (registers[instruction.second_nibble] % 100) / 10;
	memory[static_cast<uint64_t>(address_register) + 2] = (registers[instruction.second_nibble] % 100) % 10;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx55() {
	CHIP8_TRACE("Opcode: [Fx55]");

	for (uint8_t i = 0; i <= instruction.second_nibble; ++i) {
		memory[static_cast<uint64_t>(address_register) + i] = registers[i];
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Fx65() {
	CHIP8_TRACE("Opcode: [Fx65]");

	for (uint8_t i = 0; i <= instruction.second_nibble; ++i) {
		registers[i] = memory[static_cast<uint64_t>(address_register) + i];
	}

	program_counter += 2;
}

//======================================================================================================================================================
//======================================================================================================================================================

void CHIP8::execute() {
	switch (instruction.first_nibble) {
		case 0x0ui8:
			if (instruction.low_byte == 0x00E0) {
				code_00E0();
			}
			else if (instruction.low_byte == 0x00EE) {
				code_00EE();
			}
			else {
				CHIP8_ERROR("Illegal opcode: [{}].", (int)instruction.instruction);
				return;
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
			switch (instruction.fourth_nibble) {
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
					CHIP8_ERROR("Illegal opcode: [{}].", (int)instruction.instruction);
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
			if ((instruction.low_byte) == 0x9E) {
				code_Ex9E();
			}
			else if ((instruction.low_byte) == 0xA1) {
				code_ExA1();
			}
			else {
				CHIP8_ERROR("Illegal opcode: [{}].", (int)instruction.instruction);
				return;
			}
			break;
		case 0xFui8:
			switch ((instruction.low_byte)) {
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
					CHIP8_ERROR("Illegal opcode: [{}].", (int)instruction.instruction);
					break;
			}
			break;
		default:
			CHIP8_ERROR("Illegal opcode: [{}].", (int)instruction.instruction);
			break;
	}
}

//======================================================================================================================================================
//======================================================================================================================================================

void CHIP8::load_memory(const char* file_name) {
	//Font data:
	//0.				   //1.				      //2.				     //3.				    //4.				   //5.				      //6.				     //7.
	memory[80] = 0xF0ui8;  memory[85] = 0x20ui8;  memory[90] = 0xF0ui8;  memory[95] = 0xF0ui8;  memory[100] = 0x90ui8; memory[105] = 0xF0ui8; memory[110] = 0xF0ui8; memory[115] = 0xF0ui8;
	memory[81] = 0x90ui8;  memory[86] = 0x60ui8;  memory[91] = 0x10ui8;  memory[96] = 0x10ui8;  memory[101] = 0x90ui8; memory[106] = 0x80ui8; memory[111] = 0x80ui8; memory[116] = 0x10ui8;
	memory[82] = 0x90ui8;  memory[87] = 0x20ui8;  memory[92] = 0xF0ui8;  memory[97] = 0xF0ui8;  memory[102] = 0xF0ui8; memory[107] = 0xF0ui8; memory[112] = 0xF0ui8; memory[117] = 0x20ui8;
	memory[83] = 0x90ui8;  memory[88] = 0x20ui8;  memory[93] = 0x80ui8;  memory[98] = 0x10ui8;  memory[103] = 0x10ui8; memory[108] = 0x10ui8; memory[113] = 0x90ui8; memory[118] = 0x40ui8;
	memory[84] = 0xF0ui8;  memory[89] = 0x70ui8;  memory[94] = 0xF0ui8;  memory[99] = 0xF0ui8;  memory[104] = 0x10ui8; memory[109] = 0xF0ui8; memory[114] = 0xF0ui8; memory[119] = 0x40ui8;

	//8.				   //9.				      //A.				     //B.				    //C.				   //D.				      //E.				     //F.
	memory[120] = 0xF0ui8; memory[125] = 0xF0ui8; memory[130] = 0xF0ui8; memory[135] = 0xE0ui8; memory[140] = 0xF0ui8; memory[145] = 0xE0ui8; memory[150] = 0xF0ui8; memory[155] = 0xF0ui8;
	memory[121] = 0x90ui8; memory[126] = 0x90ui8; memory[131] = 0x90ui8; memory[136] = 0x90ui8; memory[141] = 0x80ui8; memory[146] = 0x90ui8; memory[151] = 0x80ui8; memory[156] = 0x80ui8;
	memory[122] = 0xF0ui8; memory[127] = 0xF0ui8; memory[132] = 0xF0ui8; memory[137] = 0xE0ui8; memory[142] = 0x80ui8; memory[147] = 0x90ui8; memory[152] = 0xF0ui8; memory[157] = 0xF0ui8;
	memory[123] = 0x90ui8; memory[128] = 0x10ui8; memory[133] = 0x90ui8; memory[138] = 0x90ui8; memory[143] = 0x80ui8; memory[148] = 0x90ui8; memory[153] = 0x80ui8; memory[158] = 0x80ui8;
	memory[124] = 0xF0ui8; memory[129] = 0xF0ui8; memory[134] = 0x90ui8; memory[139] = 0xE0ui8; memory[144] = 0xF0ui8; memory[149] = 0xE0ui8; memory[154] = 0xF0ui8; memory[159] = 0x80ui8;


	std::ifstream file(file_name, std::ios::in|std::ios::binary|std::ios::ate);

	if (file.is_open()) {
		std::streampos file_size = file.tellg();
		file.seekg(0, std::ios::beg);


		//Load memory from the 512th byte.
		file.read((char*)&memory[512], file_size);


		file.close();

		CHIP8_INFO("Loaded memory: [{}].", file_name);
	}
	else {
		CHIP8_ERROR("Failed to load memory: [{}].", file_name);
	}
}

//======================================================================================================================================================

void CHIP8::run() {
	window.create({ 640, 320 }, "CHIP-8 Emulator");

	//------------------------------------------------------------------------------------------------------------------------------------------------------

	std::thread delay_thread(
		[&delay_register = delay_register, &window = window]() {
			while (window.isOpen()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(5));

				if (delay_register > 0) {
					delay_register--;
				}
			}
		}
	);

	std::thread sound_thread(
		[&sound_register = sound_register, &window = window]() {
			while (window.isOpen()) {
				std::this_thread::sleep_for(std::chrono::milliseconds(5));

				if (sound_register > 0) {
					sound_register--;
					std::cout << "Sound.\n";
				}
			}
		}
	);

	//------------------------------------------------------------------------------------------------------------------------------------------------------

	while (window.isOpen()) {
		//Clock delay:
		//std::this_thread::sleep_for(std::chrono::milliseconds(2));

		instruction.instruction = (memory[program_counter] << 8) | memory[static_cast<uint64_t>(program_counter) + 1];

		//------------------------------------------------------------------------------------------------------------------------------------------------------

		execute();

		//------------------------------------------------------------------------------------------------------------------------------------------------------

		window.pollEvent(event);

		if (event.type == sf::Event::Closed) {
			window.close();
			CHIP8_INFO("Window closed.");
		}

		window.display();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------

	delay_thread.join();
	sound_thread.join();
}

//======================================================================================================================================================