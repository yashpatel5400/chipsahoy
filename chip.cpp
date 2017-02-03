#include "chip.h"

// all information obtained from: 
// https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description

void Chip::initialize() {
	/* Chips is laid out as follows:
	0x000-0x1FF : memory
	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	0x200-0xFFF - Program ROM and work RAM */

	// sets the program counter to where in memory the program ROM/work RAM
	pc = 0x200;

	// sets all the registers to default values and opcode since not executing
	opcode = 0;
	I	   = 0;
	pc	   = 0;

	// clear the storage items: memory, stack, graphics, and registers
	// all of the first three are 1-bit unsigned chars
	for (int i = 0; i < 4096; i++)    memory[i] = 0x0;
	for (int i = 0; i < 16; i++)      registers[i] = 0x0;
	for (int i = 0; i < 64 * 32; i++) gfx[i] = 0x0;

	// stack is a list of 2-byte shorts, so slightly different
	for (int i = 0; i < 16; i++) stack[i] = 0x00;

	// initializes the memory and registers
	for (int i = 0; i < 80; i++)
		memory[i] = chip_fontset[i];

	// resets the timers to default 0 value since not activated
	delay_timer = 0;
	sound_timer = 0;
}

void Chip::loadGame(char* name) {
	// reads from the game/ROM and loads into 0x200-0xFFF
	// starts inputting at 512, since 0x200 = 512 in decimal
	FILE*  pFile;
	long   lSize;
	char*  buffer;
	size_t result;

	pFile = fopen(name, "rb");
	if (pFile == NULL) {
		fputs("File error",stderr); 
		exit(1);
	}

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	buffer = new char[lSize];
	if (buffer == NULL) {
		fputs("Memory error",stderr); 
		exit(1);
	}

	// copy the file into the buffer:
	result = fread(buffer, 1, lSize, pFile);
	if (result != lSize) {
		fputs("Reading error",stderr); 
		exit(1);
	}

	// the whole file is now loaded in the memory buffer and can load -> mem
	for (int i = 0; i < lSize; i++)
		memory[i + 512] = buffer[i];
	
	// terminate
	fclose(pFile);
	delete[] buffer;
}
	
void Chip::emulateCycle() {
	// finds the current opcode
	// since the memory locations are 1 byte and opcodes are 2, we need to
	// combine the two contiguous ones to get current opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	// decodes the opcode i.e. determine what it does + execute
	switch (opcode & 0xF000) {
		//	Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
		case 0x0NNN: 
		break;

		// Clears the screen.
		case 0x00E0: 
		break;

		// Returns from a subroutine.
		case 0x00EE: 
		break;

		// Jumps to address NNN.
		case 0x1NNN: 
		break;

		case 0x2NNN: 
		break;

		case 0x3XNN: 
		break;

		case 0x4XNN: 
		break;

		case 0x5XY0:
		break;

		case 0x6XNN:
		break;

		case 0x7XNN:
		break;

		case 0x8XY0:
		break;

		case 0x8XY1:
		break;

		case 0x8XY2:
		break;

		case 0x8XY3:
		break;

		case 0x8XY4: 
		break;

		case 0x8XY5: 
		break;

		case 0x8XY6: 
		break;

		case 0x8XY7: 
		break;

		case 0x8XYE: 
		break;

		case 0x9XY0: 
		break;

		case 0xANNN: 
		break;

		case 0xBNNN: 
		break;

		case 0xCXNN: 
		break;

		case 0xDXYN: 
		break;

		case 0xEX9E: 
		break;

		case 0xEXA1: 
		break;

		case 0xFX07: 
		break;

		case 0xFX0A: 
		break;

		case 0xFX15: 
		break;

		case 0xFX18: 
		break;

		case 0xFX1E: 
		break;

		case 0xFX29: 
		break;

		case 0xFX33: 
		break;

		case 0xFX55: 
		break;

		case 0xFX65: 
		break;

		default:
			printf("Unknown opcode supplied: %X", opcode)
	};

	// updates the timers (i.e. decrements or resets)
}

void Chip::setKeys() {

}