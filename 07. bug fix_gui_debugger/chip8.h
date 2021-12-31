#pragma once
#ifndef CHIP8_H
#define CHIP8_H

#include "chip8const.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <string>
#include <cstring>
#include <random>
#include<ctime>


class Chip8
{
public:
	// 메모리
	unsigned char memory[CHIP8_MEM_SIZE];
	// 범용 8비트 레지스터 16개
	unsigned char V[CHIP8_NUM_GENERAL_REGS];
	// 임시 레지스터
	unsigned char TMP;
	// 메모리 주소 레지스터 MAR (16비트)
	unsigned short I;
	// 딜레이 타이머
	unsigned char DT;
	// 사운드 타이머
	unsigned char ST;
	// 프로그램 카운터
	unsigned short PC;
	// 스택 포인터
	unsigned char SP;
	// 메모리 스택
	unsigned short stack[CHIP8_STACK_LEVELS];
	// 키보드
	bool keyboard[CHIP8_NUM_KEYS];
	// 키보드 맵
	const char* keymap;
	// 디스플레이
	bool display_pixels[CHIP8_HEIGHT][CHIP8_WIDTH];




	Chip8()
	{
		//chip8 클래스 객체 인스턴스 초기화
		memset(this, 0, sizeof(this));
		//chip8 객체 메모리에 캐릭터 스프라이트 복사
		memcpy(this->memory, characterSprites, sizeof(characterSprites));
		clearDisplay();
		//키보드 매핑
		keymap = SDL_keyMap;
	}

	
	unsigned short fetch();
	void execute(unsigned short _code);
	void executeGeneral(unsigned short _code);
	void execute8000(unsigned short _code);
	void executeE000(unsigned short _code);
	void executeF000(unsigned short _code);
	void clearDisplay();
	void stackPush(unsigned short value);
	unsigned short stackPop();
	bool drawSprite(const char* ptrSprite, int px, int py, int n);
	void loadProgram(const char* data, int len);
	bool isKeyPressed(int key);
	int mapKey(char inputKey);
	char SDLKeyPressed();
	std::string decode(unsigned short _code);
	std::string decodeGeneral(unsigned short _code);
	std::string decode8000(unsigned short _code);
	std::string decodeE000(unsigned short _code);
	std::string decodeF000(unsigned short _code);
};



#endif