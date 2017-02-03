#include "chip.h"

// all information obtained from: 
// https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description

/* Chips is laid out as follows:
0x000-0x200 : memory
0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
0x200-0xFFF - Program ROM and work RAM
*/

void Chip::initialize() {
}

void Chip::loadGame(char* name) {

}
	
void Chip::emulateCycle() {
}

void Chip::setKeys() {
}