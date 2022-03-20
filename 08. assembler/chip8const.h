#pragma once
#ifndef CHIP8CONST_H
#define CHIP8CONST_H

#define CHIP8_MEM_SIZE 4096
#define CHIP8_NUM_GENERAL_REGS 16
#define CHIP8_NUM_KEYS 16
#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32
#define CHIP8_DISPLAY_ZOOM 20
#define CHIP8_STACK_LEVELS 16
#define CHIP8_WRONG -1
#define CHIP8_SPRITE_SIZE 5
#define CHIP8_PROGRAMDATA_ADDR 0x200
#define CHIP8_BINARY_MODE 0
#define CHIP8_ASSEMBLY_MODE 1
#define CHIP8_DISASSEMBLY_MODE 2

#define CHIP8_PLAY_MODE 0
#define CHIP8_STEP_MODE 1


#define CHIP8_IMG_PLAY_BUTTON_PATH "./images/play.png"
#define CHIP8_IMG_PAUSE_BUTTON_PATH "./images/pause.png"
#define CHIP8_IMG_STEP_BUTTON_PATH "./images/ff.png"

//chip8 캐릭터 스프라이트

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

const char characterSprites[]
= {
0xf0, 0x90, 0x90, 0x90, 0xf0,
0x20, 0x60, 0x20, 0x20, 0x70,
0xf0, 0x10, 0xf0, 0x80, 0xf0,
0xf0, 0x10, 0xf0, 0x10, 0xf0,
0x90, 0x90, 0xf0, 0x10, 0x10,
0xf0, 0x80, 0xf0, 0x10, 0xf0,
0xf0, 0x80, 0xf0, 0x90, 0xf0,
0xf0, 0x10, 0x20, 0x40, 0x40,
0xf0, 0x90, 0xf0, 0x90, 0xf0,
0xf0, 0x90, 0xf0, 0x10, 0xf0,
0xf0, 0x90, 0xf0, 0x90, 0x90,
0xe0, 0x90, 0xe0, 0x90, 0xe0,
0xf0, 0x80, 0x80, 0x80, 0xf0,
0xe0, 0x90, 0x90, 0x90, 0xe0,
0xf0, 0x80, 0xf0, 0x80, 0xf0,
0xf0, 0x80, 0xf0, 0x80, 0x80
};

const char SDL_keyMap[CHIP8_NUM_KEYS] = 
{ SDLK_0, SDLK_1, SDLK_2, SDLK_3, SDLK_4,
SDLK_5, SDLK_6, SDLK_7, SDLK_8, SDLK_9,
SDLK_a, SDLK_b, SDLK_c, SDLK_d, SDLK_e, SDLK_f };


const enum CHIP8_INST_TYPE
{
	LABEL_INST = 0, //not used
	ORIGIN_INST, // not used 
	DATA_INST, // not used 
	COMMENT_INST, //comment - no instruction
	SELF_INST, //self contained instruction
	REG_INST, // one reg instruction
	REG_NUM_INST, // one reg and one number instruction
	REG_REG_INST, // two reg instruction
	REGI_REG_INST, // indirect reg - reg instruction
	REG_REGI_INST, // reg - indirect reg instruction

};

#endif