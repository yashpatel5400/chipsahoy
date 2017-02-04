#include "chip.h"
#include <SDL2/SDL.h>
#include <chrono>
#include <thread>

Chip chip8;

// since Chip8 only supports 64x32 graphics output, we have to introduce
// a stretch factor to properly draw it on-screen
// each STRETCH x STRETCH block now represents one pixel
int STRETCH = 10;

// corresponds to the letters to be used for input following: 
// https://www.libsdl.org/release/SDL-1.2.15/docs/html/guideinputkeyboard.html
uint8_t keymap[16] = {
	SDLK_1, SDLK_2,  SDLK_3,  SDLK_4,
	SDLK_q, SDLK_w,  SDLK_e,  SDLK_r,
	SDLK_a, SDLK_s,  SDLK_d,  SDLK_f,
	SDLK_z, SDLK_x,  SDLK_c,  SDLK_v,
};

void confirmCreated(void* obj) {
	if (obj == NULL) {
		printf("Could not create: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: chip <ROM (w/ directory)>");
		exit(EXIT_FAILURE);
	}

	// initializes the chip and loads the game
	chip8.initialize();
	chip8.loadGame(argv[1]);

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		printf("Could not initialize SDL!");
		exit(EXIT_FAILURE);
	}

	// used for handling main game loop
	SDL_Event event;
	int quit = 0;

	// Create window
	SDL_Window* window = SDL_CreateWindow(
		"chipsahoy",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		chip8.width * STRETCH, chip8.height * STRETCH, SDL_WINDOW_SHOWN
	);
	if (window == NULL){
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	// Create renderer
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_RenderSetLogicalSize(renderer, chip8.width * STRETCH, chip8.height * STRETCH);

	// Create texture that stores frame buffer
	SDL_Texture* texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		64, 32);

	// screen buffer used for transferring from internal chip graphics storage
	// to pixel data
	uint32_t screen[64 * 32];

	// main game loop
	while (!quit) {
		chip8.emulateCycle();
		while(SDL_PollEvent(&event) ){
			switch(event.type){
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					for (int i = 0; i < 16; i++) {
						if (event.key.keysym.sym == keymap[i]) {
							chip8.keys[i] = 1;
						}
					}
					break;

				case SDL_KEYUP:
					for (int i = 0; i < 16; i++) {
						if (event.key.keysym.sym == keymap[i]) {
							chip8.keys[i] = 0;
						}
					}
					break;

				/* SDL_QUIT event (window close) */
				case SDL_QUIT:
					quit = 1;
					break;

				default:
					break;
			}
		}

		// if the refresh flag has reached refresh, updates the screen
		if (chip8.drawFlag) {
			for (int y = 0; y < chip8.height; y++) {
				for (int x = 0; x < chip8.width; x++) {
					unsigned char curPixel = chip8.gfx[y * chip8.width + x];
					screen[y * chip8.width + x] = (0x00FFFFFF * curPixel) | 0xFF000000;
				}
			}

			SDL_UpdateTexture(texture,
				NULL,  // area updating -- NULL to update entire area
				screen, // the raw pixel data
				chip8.width * sizeof(Uint32) // the number of bytes in a row of pixel data)
			);
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);

			chip8.drawFlag = false;
		}

		// matches the clock speed of the Chip8 processor
		std::this_thread::sleep_for(std::chrono::microseconds(1200));
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}