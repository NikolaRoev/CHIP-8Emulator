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
	stack_pointer--;
	program_counter = stack[stack_pointer];
	
	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_1nnn() {
	uint16_t hold = memory[program_counter] & 0x0F;
	program_counter = (hold << 8) | memory[static_cast<uint64_t>(program_counter) + 1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_2nnn() {
	stack[stack_pointer] = program_counter;
	stack_pointer++;

	uint16_t hold = memory[program_counter] & 0x0F;
	program_counter = (hold << 8) | memory[static_cast<uint64_t>(program_counter) + 1];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_3xkk() {
	if (registers[memory[program_counter] & 0x0F] == memory[static_cast<uint64_t>(program_counter) + 1]) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_4xkk() {
	if (registers[memory[program_counter] & 0x0F] != memory[static_cast<uint64_t>(program_counter) + 1]) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_5xy0() {
	if (registers[memory[program_counter] & 0x0F] == registers[static_cast<uint64_t>(memory[static_cast<uint64_t>(program_counter) + 1] >> 4)]) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_6xkk() {
	registers[memory[program_counter] & 0x0F] = memory[static_cast<uint64_t>(program_counter) + 1];
	
	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_7xkk() {
	registers[memory[program_counter] & 0x0F] += memory[static_cast<uint64_t>(program_counter) + 1];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy0() {
	registers[memory[program_counter] & 0x0F] = registers[static_cast<uint64_t>(memory[static_cast<uint64_t>(program_counter) + 1] >> 4)];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy1() {
	registers[memory[program_counter] & 0x0F] |= registers[static_cast<uint64_t>(memory[static_cast<uint64_t>(program_counter) + 1] >> 4)];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy2() {
	registers[memory[program_counter] & 0x0F] &= registers[static_cast<uint64_t>(memory[static_cast<uint64_t>(program_counter) + 1] >> 4)];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy3() {
	registers[memory[program_counter] & 0x0F] ^= registers[static_cast<uint64_t>(memory[static_cast<uint64_t>(program_counter) + 1] >> 4)];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy4() {
	uint16_t temp_Vx = registers[memory[program_counter] & 0x0F];
	uint16_t temp_Vy = registers[static_cast<uint64_t>(memory[static_cast<uint64_t>(program_counter) + 1] >> 4)];

	temp_Vx += temp_Vy;

	registers[0xF] = (temp_Vx & 0x100) == 0x100;

	temp_Vx &= 0xFF;

	registers[memory[program_counter] & 0x0F] = static_cast<uint8_t>(temp_Vx);

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy5() {
	if (registers[memory[program_counter] & 0x0F] > registers[static_cast<uint64_t>(memory[static_cast<uint64_t>(program_counter) + 1] >> 4)]) {
		registers[0xF] = 1;
	}
	else {
		registers[0xF] = 0;
	}

	registers[memory[program_counter] & 0x0F] -= registers[static_cast<uint64_t>(memory[static_cast<uint64_t>(program_counter) + 1] >> 4)];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy6() {
	registers[0xF] = registers[memory[program_counter] & 0x0F] & 0x1;

	registers[memory[program_counter] & 0x0F] >>= 1;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xy7() {
	if (registers[memory[program_counter] & 0x0F] < registers[static_cast<uint64_t>(memory[static_cast<uint64_t>(program_counter) + 1] >> 4)]) {
		registers[0xFui8] = 1;
	}
	else {
		registers[0xFui8] = 0;
	}

	registers[memory[program_counter] & 0x0F] = registers[static_cast<uint64_t>(memory[static_cast<uint64_t>(program_counter) + 1] >> 4)] - registers[memory[program_counter] & 0x0F];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_8xyE() {
	registers[0xF] = (registers[memory[program_counter] & 0x0F] & 0x80) == 0x80;

	registers[memory[program_counter] & 0x0F] <<= 1;

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_9xy0() {
	if (registers[memory[program_counter] & 0x0F] != registers[static_cast<uint64_t>(memory[static_cast<uint64_t>(program_counter) + 1] >> 4)]) {
		program_counter += 2;
	}

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Annn() {
	uint16_t hold = memory[program_counter] & 0x0F;
	address_register = (hold << 8) | memory[static_cast<uint64_t>(program_counter) + 1];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Bnnn() {
	uint16_t hold = memory[program_counter] & 0x0F;
	program_counter = ((hold << 8) | memory[static_cast<uint64_t>(program_counter) + 1]) + registers[0x0];
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Cxkk() {
	registers[memory[program_counter] & 0x0F] = static_cast<uint8_t>(rand() % 256) & memory[static_cast<uint64_t>(program_counter) + 1];

	program_counter += 2;
}

//------------------------------------------------------------------------------------------------------------------------------------------------------

void CHIP8::code_Dxyn() {
	uint8_t x = registers[memory[program_counter] & 0b0000'1111];
	uint8_t y = registers[memory[static_cast<uint64_t>(program_counter) + 1] >> 4];
	uint8_t n = memory[static_cast<uint64_t>(program_counter) + 1] & 0b0000'1111;

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

	registers[0xFui8] = collision;

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
	sf::Keyboard::Key key{ sf::Keyboard::Key::Unknown };

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
	sf::Keyboard::Key key{ sf::Keyboard::Key::Unknown };

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
	registers[memory[program_counter] & 0x0F] = delay_register;

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
	address_register = (registers[memory[program_counter] & 0b0000'1111] * 5) + 0x50;

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
				std::this_thread::sleep_for(std::chrono::milliseconds(5));

				if (delay_register > 0) {
					delay_register--;
				}
			}
		}
	);

	std::thread sound_thread(
		[&sound_register = sound_register, &running]() {
			while (running) {
				std::this_thread::sleep_for(std::chrono::milliseconds(5));

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

		uint16_t instruction = (memory[program_counter] << 8) | memory[static_cast<uint64_t>(program_counter) + 1];

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
			running = false;
		}

		window.display();
	}

	//------------------------------------------------------------------------------------------------------------------------------------------------------

	delay_thread.join();
	sound_thread.join();
}

//======================================================================================================================================================