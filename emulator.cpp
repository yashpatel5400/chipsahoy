#include "chip.h"

int main(int argc, char **argv) {
	chip myChip;

	// set up graphics and input
	setupGraphics();
	setupInput();	

	// initializes the chip and loads the game
	myChip.initialize();
	myChip.loadGame("pong");

	// game loop to run the game
	while (true) {
		// sees the update of the emulator in current cycle
		myChip.emulateCycle();

		// if the refresh flag has reached refresh, updates the screen
		if (myChip.drawFlag)
			drawGraphics();

		// updates the chip by reading in key input
		myChip.setKeys();
	}

	return 0;
}