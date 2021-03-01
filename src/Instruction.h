#pragma once
#include <cstdint>

//======================================================================================================================================================

struct Instruction {
	union {
		struct {
			uint16_t low_byte : 8;
			uint16_t high_byte : 8;
		};
		struct {
			uint16_t address : 12;
			uint16_t unused : 4;
		};
		struct {
			uint16_t fourth_nibble : 4;
			uint16_t third_nibble : 4;
			uint16_t second_nibble : 4;
			uint16_t first_nibble : 4;
		};
		uint16_t instruction;
	};
};

//======================================================================================================================================================