#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <curses.h>

class Chip {
private:
	unsigned char* chip8_fontset;

	// current opcode -- two bytes in 8-chip
	unsigned short opcode;

	// has 4096 bytes of memory
	unsigned char memory[4096];

	// register is 16 8-bit (1 byte) registers
	unsigned char registers[16];

	// index register -- 16-bit register
	unsigned short I;
	
	// program counter -- 16-bit register
	unsigned short pc;	

	// two timers: delay and sound, both counting down from 60 for refresh
	unsigned char delayTimer;
	unsigned char soundTimer;

	// stack and stack pointer to determine where coming from in goto
	unsigned short stack[16];
	unsigned short sp;

public:
	Chip();
	~Chip();

	void initialize();
	void loadGame(char* name);
	void emulateCycle();

	// flag indicating whether or not the chip graphics should be displayed
	bool drawFlag; 

	// keypad used for inputting into system: hex
	unsigned char keys[16];

	// width and height of the chip display graphics output
	int width;
	int height;

	// outputs graphics display of black/white (binary = 1 byte) of 64 x 32
	unsigned char gfx[64 * 32];
};