#pragma once

class Chip {
private:
	// an opcode -- type chosen since they are all two bytes in 8-chip
	unsigned short opcode;

	// has 4096 bytes of memory
	unsigned char memory[4096];

	// register is 16 8-bit (1 byte) registers
	unsigned char registers[16];

	// index register -- 16-bit register
	unsigned short I;
	
	// program counter -- 16-bit register
	unsigned short pc;	

	// outputs graphics display of black/white (binary = 1 byte)
	// of 64 x 32
	unsigned char gfx[64 * 32];

	// two timers: delay and sound, both counting down from 60 for refresh
	unsigned char delay_timer;
	unsigned char sound_timer;

	// stack and stack pointer to determine where coming from in goto
	unsigned short stack[16];
	unsigned short sp;

	// keypad used for inputting into system: hex
	unsigned char key[16];

public:
	void initialize();
	void loadGame(char* name);
	void emulateCycle();
	void setKeys();
}