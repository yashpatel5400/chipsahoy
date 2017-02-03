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
		case 0x0000:
			switch (opcode & 0x000F) {
				// 0x00E0: Clears the screen.
				case 0x0000: 
				break;

				// 0x00EE: Returns from a subroutine.
				case 0x000E: 
				break;

				//	0x0NNN: Calls RCA 1802 program at address NNN. Not necessary for most ROMs.
				default:
			}
		break;

		// 0x1NNN: Jumps to address NNN.
		case 0x1000:
		break;

		// 0x2NNN: Calls subroutine at NNN.
		case 0x2000: 
		break;

		// 0x3XNN: Skips the next instruction if VX equals NN. (Usually the next 
		// instruction is a jump to skip a code block)
		case 0x3000: 
		break;

		// 0x4XNN: Skips the next instruction if VX doesn't equal NN. (Usually 
		// the next instruction is a jump to skip a code block)
		case 0x4000: 
		break;

		// 0x5XY0Skips the next instruction if VX equals VY. (Usually the 
		// next instruction is a jump to skip a code block)
		case 0x5000:
		break;

		// 0x6XNN: Sets VX to NN
		case 0x6000:
		break;

		// 0x7XNN: Adds NN to VX
		case 0x7000:
		break;

		case 0x8000:
			switch (opcode & 0x000F) {
				// 0x8XY0: Sets VX to the value of VY
				case 0x0000:
				break;

				// 0x8XY1: Sets VX to VX or VY. (Bitwise OR operation)
				case 0x0001:
				break;

				// 0x8XY2: Sets VX to VX and VY. (Bitwise AND operation)
				case 0x0002:
				break;

				// 0x8XY3: Sets VX to VX xor VY
				case 0x0003:
				break;

				// 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, 
				// and to 0 when there isn't.
				case 0x0004: 
				break;

				// 0x8XY5: VY is subtracted from VX VF is set to 0 when there's a 
				// borrow, and 1 when there isn't
				case 0x0005: 
				break;

				// 0x8XY6: Shifts VX right by one VF is set to the value of the least 
				// significant bit of VX before the shift
				case 0x0006: 
				break;

				// 0x8XY7: Sets VX to VY minus VX VF is set to 0 when there's a borrow, 
				// and 1 when there isn't
				case 0x0007: 
				break;

				// 0x8XYE: Shifts VX left by one VF is set to the value of the most 
				// significant bit of VX before the shift
				case 0x000E: 
				break;

				default:
					printf("Unknown opcode supplied: %X", opcode);
			}
		break;

		// 0x9XY0: Skips the next instruction if VX doesn't equal VY (Usually the 
		// next instruction is a jump to skip a code block)
		case 0x9000: 
		break;

		// 0xANNN: Sets I to the address NNN
		case 0xA000: 
		break;

		// 0xBNNN: Jumps to the address NNN plus V0
		case 0xB000: 
		break;

		// 0xCXNN: Sets VX to the result of a bitwise and operation on a random number 
		// (Typically: 0 to 255) and NN
		case 0xC000: 
		break;

		// 0xDXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and 
		// a height of N pixels Each row of 8 pixels is read as bit-coded starting 
		// from memory location I; I value doesn’t change after the execution of 
		// this instruction As described above, VF is set to 1 if any screen pixels 
		// are flipped from set to unset when the sprite is drawn, and to 0 if 
		// that doesn’t happen
		case 0xD000: 
		break;

		case 0xE000: 
			switch (opcode & 0x000F) {
				// 0xEXA1: Skips the next instruction if the key stored in 
				// VX isn't pressed (Usually the next instruction is a jump
				// to skip a code block)
				case 0x0001: 
				break;

				// 0xEX9E: Skips the next instruction if the key 
				// stored in VX is pressed (Usually the next instruction is a 
				// jump to skip a code block)
				case 0x000E: 
				break;

				default:
					printf("Unknown opcode supplied: %X", opcode);
			}
		break;

		case 0xF000:
			switch (opcode & 0x00FF) {
				// 0xFX07: Sets VX to the value of the delay timer
				case 0x0007: 
				break;

				// 0xFX0A: A key press is awaited, and then stored in VX 
				// (Blocking Operation All instruction halted until next key event)
				case 0x000A: 
				break;

				// 0xFX15: Sets the delay timer to VX
				case 0x0015: 
				break;

				// 0xFX18: Sets the sound timer to VX
				case 0x0018: 
				break;

				// 0xFX1E: Adds VX to I
				case 0x001E: 
				break;

				// 0xFX29: Sets I to the location of the sprite for the 
				// character in VX Characters 0-F (in hexadecimal) are 
				// represented by a 4x5 font
				case 0x0029: 
				break;

				// 0xFX33: Stores the binary-coded decimal representation of VX, w/ most
				// significant of three digits at the address in I, the middle digit 
				// at I plus 1, and the least significant digit at I plus 2 (In other 
				// words, take the decimal representation of VX, place the hundreds digit
				// in memory at location in I, the tens digit at location I+1, and the 
				// ones digit at location I+2)
				case 0x0033: 
				break;

				// 0xFX55: Stores V0 to VX (including VX) in memory starting at address I
				case 0x0055: 
				break;

				// 0xFX65: Fills V0 to VX (including VX) with values from memory 
				// starting at address I
				case 0x0065: 
				break;

				default:
					printf("Unknown opcode supplied: %X", opcode);
			}
		break;

		default:
			printf("Unknown opcode supplied: %X", opcode);
	}

	// updates the timers (i.e. decrements or resets)
}

void Chip::setKeys() {

}