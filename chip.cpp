#include "chip.h"

// all information obtained from: 
// https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description

Chip::Chip() {}
Chip::~Chip () {}

void Chip::initialize() {
	// initializes the display sizes -- fixed by Chip
	width  = 64;
	height = 32;

	/* Chips is laid out as follows:
	0x000-0x1FF : memory
	0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
	0x200-0xFFF - Program ROM and work RAM */
	unsigned char chip8_fontset[80] = { 
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	// sets the program counter to where in memory the program ROM/work RAM
	pc = 0x200;

	// sets all the registers to default values and opcode since not executing
	opcode = 0;
	I	   = 0;
	sp	   = 0;

	// clear the storage items: memory, stack, graphics, and registers
	for (int i = 0; i < 4096; i++)    memory[i] = 0;
	for (int i = 0; i < 16; i++)      keys[i] = 0;
	for (int i = 0; i < 16; i++)      registers[i] = 0;
	for (int i = 0; i < 64 * 32; i++) gfx[i] = 0;
	for (int i = 0; i < 16; i++)      stack[i] = 0;

	// initializes the memory and registers
	for (int i = 0; i < 80; i++)
		memory[i] = chip8_fontset[i];

	// resets the timers to default 0 value since not activated
	delayTimer = 0;
	soundTimer = 0;
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
					for (int i = 0; i < 64 * 32; i++) gfx[i] = 0;
					drawFlag = true;
					pc += 2;
				break;

				// 0x00EE: Returns from a subroutine.
				case 0x000E: 
					sp--;
					pc = stack[sp];

					// after returning from the subroutine, have to move to next
					// instruction otherwise will repeat the subroutine
					pc += 2;
				break;
			}
		break;

		// 0x1NNN: Jumps to address NNN.
		case 0x1000:
			pc = opcode & 0x0FFF;
		break;

		// 0x2NNN: Calls subroutine at NNN.
		case 0x2000:
			stack[sp] = pc;
			sp++;
			pc = opcode & 0x0FFF;
		break;

		// 0x3XNN: Skips the next instruction if VX equals NN. (Usually the next 
		// instruction is a jump to skip a code block)
		case 0x3000: {
			unsigned char VX = registers[(opcode & 0x0F00) >> 8];
			unsigned char NN = opcode & 0x00FF;
			if (VX == NN) pc += 4;
			else pc += 2;
		}
		break;

		// 0x4XNN: Skips the next instruction if VX doesn't equal NN. (Usually 
		// the next instruction is a jump to skip a code block)
		case 0x4000: {
			unsigned char VX = registers[(opcode & 0x0F00) >> 8];
			unsigned char NN = opcode & 0x00FF;
			if (VX != NN) pc += 4;
			else pc += 2;
		}
		break;

		// 0x5XY0Skips the next instruction if VX equals VY. (Usually the 
		// next instruction is a jump to skip a code block)
		case 0x5000: {
			unsigned char VX = registers[(opcode & 0x0F00) >> 8];
			unsigned char VY = registers[(opcode & 0x00F0) >> 4];
			if (VX == VY) pc += 4;
			else pc += 2;
		}
		break;

		// 0x6XNN: Sets VX to NN
		case 0x6000: {
			unsigned char NN = opcode & 0x00FF;
			registers[(opcode & 0x0F00) >> 8] = NN;
			pc += 2;
		}
		break;

		// 0x7XNN: Adds NN to VX
		case 0x7000: {
			unsigned char NN = opcode & 0x00FF;
			registers[(opcode & 0x0F00) >> 8] += NN;
			pc += 2;
		}
		break;

		case 0x8000: {
			unsigned char VX = registers[(opcode & 0x0F00) >> 8];
			unsigned char VY = registers[(opcode & 0x00F0) >> 4];

			switch (opcode & 0x000F) {
				// 0x8XY0: Sets VX to the value of VY
				case 0x0000:
					registers[(opcode & 0x0F00) >> 8] = VY;
				break;

				// 0x8XY1: Sets VX to VX or VY. (Bitwise OR operation)
				case 0x0001:
					registers[(opcode & 0x0F00) >> 8] |= VY;
				break;

				// 0x8XY2: Sets VX to VX and VY. (Bitwise AND operation)
				case 0x0002:
					registers[(opcode & 0x0F00) >> 8] &= VY;
				break;

				// 0x8XY3: Sets VX to VX xor VY
				case 0x0003:
					registers[(opcode & 0x0F00) >> 8] ^= VY;
				break;

				// 0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, 
				// and to 0 when there isn't.
				case 0x0004: 
					if (VX + VY > 0xFF) registers[0xF] = 1;
					else registers[0xF] = 0;
					registers[(opcode & 0x0F00) >> 8] += VY;
				break;

				// 0x8XY5: VY is subtracted from VX VF is set to 0 when there's a 
				// borrow, and 1 when there isn't
				case 0x0005:
					if (VY < VX) registers[0xF] = 1;
					else registers[0xF] = 0;
					registers[(opcode & 0x0F00) >> 8] -= VY;
				break;

				// 0x8XY6: Shifts VX right by one VF is set to the value of the least 
				// significant bit of VX before the shift
				case 0x0006: 
					registers[0xF] = (VX & 0x1);
					registers[(opcode & 0x0F00) >> 8] >>= 1;
				break;

				// 0x8XY7: Sets VX to VY minus VX VF is set to 0 when there's a borrow, 
				// and 1 when there isn't
				case 0x0007: 
					if (VX < VY) registers[0xF] = 0;
					else registers[0xF] = 1;
					registers[(opcode & 0x0F00) >> 8] = VY - VX;
				break;

				// 0x8XYE: Shifts VX left by one VF is set to the value of the most 
				// significant bit of VX before the shift
				case 0x000E: 
					registers[0xF] = VX >> 7;
					registers[(opcode & 0x0F00) >> 8] <<= 1;
				break;

				default:
					printf("Unknown opcode supplied: %X", opcode);
			}
			pc += 2;
		}
		break;

		// 0x9XY0: Skips the next instruction if VX doesn't equal VY (Usually the 
		// next instruction is a jump to skip a code block)
		case 0x9000: {
			unsigned char VX = registers[(opcode & 0x0F00) >> 8];
			unsigned char VY = registers[(opcode & 0x00F0) >> 4];
			if (VX != VY) pc += 4;
			else pc += 2;
		}
		break;

		// 0xANNN: Sets I to the address NNN
		case 0xA000: 
			I = opcode & 0xFFF;
			pc += 2;
		break;

		// 0xBNNN: Jumps to the address NNN plus V0
		case 0xB000:
			pc = (opcode & 0x0FFF) + registers[0];
		break;

		// 0xCXNN: Sets VX to the result of a bitwise and operation on a random number 
		// (Typically: 0 to 255) and NN
		case 0xC000: {
			srand(time(NULL));

			// random number between 0 and 255
			registers[(opcode & 0x0F00) >> 8] =	(opcode & 0x00FF) & (rand() % 0xFF);
			pc += 2;
		}
		break;

		// 0xDXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and 
		// a height of N pixels Each row of 8 pixels is read as bit-coded starting 
		// from memory location I; I value doesn’t change after the execution of 
		// this instruction As described above, VF is set to 1 if any screen pixels 
		// are flipped from set to unset when the sprite is drawn, and to 0 if 
		// that doesn’t happen
		// NOTE: code taken from the tutorial
		case 0xD000: {
			unsigned char VX = registers[(opcode & 0x0F00) >> 8];
			unsigned char VY = registers[(opcode & 0x00F0) >> 4];
			unsigned char N  = opcode & 0x000F;

			bool unsetSprite = false;

			for (int y = 0; y < N; y++) {
				unsigned curPixel = memory[I + y];
				for (int x = 0; x < 8; x++) {
					if((curPixel & (0x80 >> x)) != 0) {
						// 64 comes from the fact that screen is 64x32
						if (gfx[VX + x + (VY + y) * 64] == 1) unsetSprite = true;
						gfx[VX + x + (VY + y) * 64] ^= 1;
					}
				}
			}

			if (unsetSprite) registers[0xF] = 1;
			else registers[0xF] = 0;
			drawFlag = true;
			pc += 2;
		}
		break;

		case 0xE000: 
			switch (opcode & 0x000F) {
				// 0xEXA1: Skips the next instruction if the key stored in 
				// VX isn't pressed (Usually the next instruction is a jump
				// to skip a code block)
				case 0x0001: 
					if (keys[registers[(opcode & 0x0F00) >> 8]] == 0) pc += 4;
					else pc += 2;
				break;

				// 0xEX9E: Skips the next instruction if the key 
				// stored in VX is pressed (Usually the next instruction is a 
				// jump to skip a code block)
				case 0x000E:
					if (keys[registers[(opcode & 0x0F00) >> 8]] != 0) pc += 4;
					else pc += 2; 
				break;

				default:
					printf("Unknown opcode supplied: %X", opcode);
			}
		break;

		case 0xF000:
			switch (opcode & 0x00FF) {
				// 0xFX07: Sets VX to the value of the delay timer
				case 0x0007: 
					registers[(opcode & 0x0F00) >> 8] = delayTimer;
				break;

				// 0xFX0A: A key press is awaited, and then stored in VX 
				// (Blocking Operation All instruction halted until next key event)
				case 0x000A: {
					bool keyPressed = false;

					for (int i = 0; i < 16; i++) {
						if (keys[i] != 0) {
							registers[(opcode & 0x0F00) >> 8] = i;
							keyPressed = true;
						}
					}

					// didn't receive input: runs same command again to see if
					// gets any input (i.e. does not change program counter)
					if (!keyPressed) return;
				}
				break;

				// 0xFX15: Sets the delay timer to VX
				case 0x0015: 
					delayTimer = registers[(opcode & 0x0F00) >> 8];
				break;

				// 0xFX18: Sets the sound timer to VX
				case 0x0018:
					soundTimer = registers[(opcode & 0x0F00) >> 8];
				break;

				// 0xFX1E: Adds VX to I
				case 0x001E:
					if (I + registers[(opcode & 0x0F00) >> 8] > 0xFFF)
						registers[0xF] = 1;
					else registers[0xF] = 0;

					I += registers[(opcode & 0x0F00) >> 8];
				break;

				// 0xFX29: Sets I to the location of the sprite for the 
				// character in VX Characters 0-F (in hexadecimal) are 
				// represented by a 4x5 font
				case 0x0029:
					I = registers[(opcode & 0x0F00) >> 8] * 0x5;
				break;

				// 0xFX33: Stores the binary-coded decimal representation of VX, w/ most
				// significant of three digits at the address in I, the middle digit 
				// at I plus 1, and the least significant digit at I plus 2 (In other 
				// words, take the decimal representation of VX, place the hundreds digit
				// in memory at location in I, the tens digit at location I+1, and the 
				// ones digit at location I+2)
				case 0x0033: {
					int num = registers[(opcode & 0x0F00) >> 8];
					memory[I]   = num / 100;
					memory[I+1] = (num - memory[I] * 100) / 10;
					memory[I+2] = (num - memory[I] * 100 - memory[I+1] * 10);
				}
				break;

				// 0xFX55: Stores V0 to VX (including VX) in memory starting at address I
				case 0x0055: {
					unsigned char end = ((opcode & 0x0F00) >> 8);
					for (int offset = 0; offset < end; offset++)
						memory[I + offset] = registers[offset];

					I += ((opcode & 0x0F00) >> 8) + 1;
				}
				break;

				// 0xFX65: Fills V0 to VX (including VX) with values from memory 
				// starting at address I
				case 0x0065: {
					unsigned char end = ((opcode & 0x0F00) >> 8);
					for (int offset = 0; offset < end; offset++)
						registers[offset] = memory[I + offset];

					I += ((opcode & 0x0F00) >> 8) + 1;
				}
				break;

				default:
					printf("Unknown opcode supplied: %X", opcode);
			}
			pc += 2;
		break;

		default:
			printf("Unknown opcode supplied: %X", opcode);
	}

	// updates the timers (i.e. decrements or resets)
	if (delayTimer > 0)
		delayTimer--;

	if (soundTimer > 0) {
		printf("BEEP!\n");
		soundTimer--;
	}
}