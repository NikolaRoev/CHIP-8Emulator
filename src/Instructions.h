#pragma once
#include <cstdint>

//======================================================================================================================================================

enum Instructions : uint16_t {
	CLS			= 0x00E0,
	RET			= 0x00EE,
	JP_addr		= 0x1000,
	CALL_addr	= 0x2000,
	SE_Vx_byte	= 0x3000,
	SNE_Vx_byte = 0x4000,
	SE_Vx_Vy	= 0x5000,
	LD_Vx_byte	= 0x6000,
	ADD_Vx_byte = 0x7000,
	LD_Vx_Vy	= 0x8000,
	OR_Vx_Vy	= 0x8001,
	AND_Vx_Vy	= 0x8002,
	XOR_Vx_Vy	= 0x8003,
	ADD_Vx_Vy	= 0x8004,
	SUB_Vx_Vy	= 0x8005,
	SHR_Vx		= 0x8006,
	SUBN_Vx_Vy	= 0x8007,
	SHL_Vx		= 0x800E,
	SNE_Vx_Vy	= 0x9000,
	LD_I_addr	= 0xA000,
	JP_V0_addr	= 0xB000,
	RND_Vx_byte = 0xC000,
	DRW_Vx_Vy_n = 0xD000,
	SKP_Vx		= 0xE09E,
	SKNP_Vx		= 0xE0A1,
	LD_Vx_DT	= 0xF007,
	LD_Vx_K		= 0xF00A,
	LD_DT_Vx	= 0xF015,
	LD_ST_Vx	= 0xF018,
	ADD_I_Vx	= 0xF01E,
	LD_F_Vx		= 0xF029,
	LD_B_Vx		= 0xF033,
	LD_aI_Vx	= 0xF055,
	LD_Vx_aI	= 0xF065,
};

//======================================================================================================================================================