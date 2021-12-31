#pragma once
#ifndef CHIP8_H
#define CHIP8_H

#include "chip8const.h"
#include <random>
#include<ctime>


//struct chip8
//{
//	// �޸�
//	unsigned char memory[CHIP8_MEM_SIZE];
//	// ���� 8��Ʈ �������� 16��
//	unsigned char V[CHIP8_NUM_GENERAL_REGS];
//	// �ӽ� ��������
//	unsigned short TMP = 0;
//	// �޸� �ּ� �������� MAR (16��Ʈ)
//	unsigned short I = 0;
//	// ������ Ÿ�̸�
//	unsigned char DT = 0;
//	// ���� Ÿ�̸�
//	unsigned char ST = 0;
//	// ���α׷� ī����
//	unsigned short PC = 0;
//	// ���� ������
//	unsigned char SP = 0;
//	// �޸� ����
//	unsigned short stack[CHIP8_STACK_LEVELS];
//	// Ű����
//	bool keyboard[CHIP8_NUM_KEYS];
//	// Ű���� ��
//	char* keymap;
//	// ���÷���
//	bool display_pixels[CHIP8_HEIGHT][CHIP8_WIDTH];
//
//
//};

class Chip8
{
public:
	// �޸�
	unsigned char memory[CHIP8_MEM_SIZE];
	// ���� 8��Ʈ �������� 16��
	unsigned char V[CHIP8_NUM_GENERAL_REGS];
	// �ӽ� ��������
	unsigned short TMP;
	// �޸� �ּ� �������� MAR (16��Ʈ)
	unsigned short I;
	// ������ Ÿ�̸�
	unsigned char DT;
	// ���� Ÿ�̸�
	unsigned char ST;
	// ���α׷� ī����
	unsigned short PC;
	// ���� ������
	unsigned char SP;
	// �޸� ����
	unsigned short stack[CHIP8_STACK_LEVELS];
	// Ű����
	bool keyboard[CHIP8_NUM_KEYS];
	// Ű���� ��
	const char* keymap;
	// ���÷���
	bool display_pixels[CHIP8_HEIGHT][CHIP8_WIDTH];

	Chip8()
	{
		//chip8 Ŭ���� ��ü �ν��Ͻ� �ʱ�ȭ
		memset(this, 0, sizeof(this));
		//chip8 ��ü �޸𸮿� ĳ���� ��������Ʈ ����
		memcpy(this->memory, characterSprites, sizeof(characterSprites));
		clearDisplay();
		//Ű���� ����
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
};



#endif