#include "chip.h"
#include <SDL2/SDL.h>

Chip chip8;

// since Chip8 only supports 64x32 graphics output, we have to introduce
// a stretch factor to properly draw it on-screen
// each STRETCH x STRETCH block now represents one pixel
int STRETCH = 10;

// corresponds to the letters to be used for input following: 
// https://www.libsdl.org/release/SDL-1.2.15/docs/html/guideinputkeyboard.html
uint8_t keypad[16] = {
	SDLK_1, SDLK_2,  SDLK_3,  SDLK_4,
	SDLK_q, SDLK_w,  SDLK_e,  SDLK_r,
	SDLK_a, SDLK_s,  SDLK_d,  SDLK_f,
	SDLK_z, SDLK_x,  SDLK_c,  SDLK_v,
}

void confirmCreated(void* obj) {
	if (obj == NULL) {
		printf("Could not create: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}
}

void initializeGraphics(SDL_Window* window, SDL_Renderer* renderer, 
	SDL_Texture *texture) {
	
	window = SDL_CreateWindow("chipsahoy", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, // initial positions
		chip8.width * STRETCH, chip8.height * STRETCH,  // width + height
		SDL_WINDOW_SHOWN
	);
	confirmCreated(window);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	confirmCreated(renderer);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, 
		SDL_TEXTUREACCESS_TARGET, 1024, 768); // width and height;
	confirmCreated(texture);
}

int main(int argc, char **argv) {
	if (!SDL_Init(SDL_INIT_EVERYTHING)) {
		printf("Could not initialize SDL!");
		exit(EXIT_FAILURE);
	}

	// used for handling main game loop
	SDL_Event event;
	int quit = 0;

	// declare all objects used for rendering to be initialized
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture *texture;
	initializeGraphics(window, renderer, texture);

	// initializes the chip and loads the game
	chip8.initialize();
	chip8.loadGame((char*) "invaders.c8");

	// main game loop
	while (!quit) {
		chip8.emulateCycle();
		printf("emulated\n");

		while(SDL_PollEvent(&event) ){
			switch(event.type){
				case SDL_KEYDOWN:
					if (e.key.keysym.sym == SDLK_ESCAPE) quit = 1;
					for (int i = 0; i < 16; i++) {
						if (e.key.keysym.sym == keypad[i]) {
							chip8.keys[i] = 1;
						}
					}

					break;

				case SDL_KEYUP:
					for (int i = 0; i < 16; i++) {
						if (e.key.keysym.sym == keypad[i]) {
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
			SDL_RenderClear(renderer);

			for (int y = 0; y < chip8.height; y++) {
				for (int x = 0; x < chip8.width; x++) {
					SDL_Rect curPixel;
					curPixel.w = STRETCH;
					curPixel.h = STRETCH;
					curPixel.x = x * STRETCH;
					curPixel.y = y * STRETCH;

					if (chip8.gfx[y * chip8.width + x] == 1)
						glColor3f(1.0f, 1.0f, 1.0f);
					else glColor3f(0.0f, 0.0f, 0.0f);
				}
			}

			chip8.drawFlag = false;
		}
	}

	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}