#pragma once
#include <cstdint>

#include <array>
#include <unordered_map>

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"


//======================================================================================================================================================

class CHIP8 {
private:
	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//Memory:
	std::array<uint8_t, 4096> memory{};
	std::array<uint16_t, 16>  stack{};
	//------------------------------------------------------------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//Registers:
	uint16_t address_register{};  //I register.

	uint8_t delay_register{};
	uint8_t sound_register{};

	uint16_t program_counter{512};
	uint8_t stack_pointer{};

	std::unordered_map<uint8_t, uint8_t> registers{
		{ 0x0ui8, 0ui8 },		//Register V0.
		{ 0x1ui8, 0ui8 },		//Register V1.
		{ 0x2ui8, 0ui8 },		//Register V2.
		{ 0x3ui8, 0ui8 },		//Register V3.
		{ 0x4ui8, 0ui8 },		//Register V4.
		{ 0x5ui8, 0ui8 },		//Register V5.
		{ 0x6ui8, 0ui8 },		//Register V6.
		{ 0x7ui8, 0ui8 },		//Register V7.
		{ 0x8ui8, 0ui8 },		//Register V8.
		{ 0x9ui8, 0ui8 },		//Register V9.
		{ 0xAui8, 0ui8 },		//Register VA.
		{ 0xBui8, 0ui8 },		//Register VB.
		{ 0xCui8, 0ui8 },		//Register VC.
		{ 0xDui8, 0ui8 },		//Register VD.
		{ 0xEui8, 0ui8 },		//Register VE.
		{ 0xFui8, 0ui8 }		//Register VF.
	};
	//------------------------------------------------------------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//Screen:
	std::array<std::array<bool, 64>, 32> screen{};
	//------------------------------------------------------------------------------------------------------------------------------------------------------

	//------------------------------------------------------------------------------------------------------------------------------------------------------
	//Opcodes:

	void code_00E0();
	void code_00EE();
	void code_1nnn();
	void code_2nnn();
	void code_3xkk();
	void code_4xkk();
	void code_5xy0();
	void code_6xkk();
	void code_7xkk();
	void code_8xy0();
	void code_8xy1();
	void code_8xy2();
	void code_8xy3();
	void code_8xy4();
	void code_8xy5();
	void code_8xy6();
	void code_8xy7();
	void code_8xyE();
	void code_9xy0();
	void code_Annn();
	void code_Bnnn();
	void code_Cxkk();
	void code_Dxyn(sf::RenderWindow& window);
	void code_Ex9E();
	void code_ExA1();
	void code_Fx07();
	void code_Fx0A();
	void code_Fx15();
	void code_Fx18();
	void code_Fx1E();
	void code_Fx29();
	void code_Fx33();
	void code_Fx55();
	void code_Fx65();

	//------------------------------------------------------------------------------------------------------------------------------------------------------

public:
	bool load_memory(const char* file_name);

	void execute();
};

//======================================================================================================================================================