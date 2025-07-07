#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "EmulatePCB.h"

#define KEY_COIN 1
#define KEY_LEFT 2
#define KEY_RIGHT 3
#define KEY_FIRE 4
#define KEY_START 5
#define KEY_PAUSE 6

int main() {
	ExtInstructions* ins = InitExt();
	State8080* state = InitMachine();
	unsigned char *framebuffer = &(state->memory[0x2400]);

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Event e;
	SDL_Init(SDL_INIT_VIDEO);
	bool running = true;

	SDL_CreateWindowAndRenderer(256 * 4, 224 * 4, 0, &window, &renderer); //Space invaders screen output is 256x244
	SDL_RenderSetScale(renderer, 4, 4);

	while (running) {
		CPUIncrement(state, ins);

		
		while(SDL_PollEvent(&e)) {
			switch(e.type) {
				case(SDL_QUIT):
					running = false;
					break;
				case(SDL_KEYDOWN):
					switch(e.key.keysym.sym) {
						case SDLK_5:
							KeyDown(KEY_COIN); break;
						case SDLK_a:
							KeyDown(KEY_LEFT); break;
						case SDLK_d:
							KeyDown(KEY_RIGHT); break;
						case SDLK_u:
							KeyDown(KEY_FIRE); break;
						case SDLK_1:
							KeyDown(KEY_START); break;
						case SDLK_p:
							KeyDown(KEY_PAUSE); break;
						default:
							break;
					}
					break;
				case(SDL_KEYUP):
					switch(e.key.keysym.sym) {
						case SDLK_5:
							KeyUp(KEY_COIN); break;
						case SDLK_a:
							KeyUp(KEY_LEFT); break;
						case SDLK_d:
							KeyUp(KEY_RIGHT); break;
						case SDLK_u:
							KeyUp(KEY_FIRE); break;
						case SDLK_1:
							KeyUp(KEY_START); break;
						default:
							break;
					}
					break;
					
			}
		}

		SDL_SetRenderDrawColor(renderer,0,0,0,255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer,255,255,255,255);
		for (int i=0; i<256; i+=8) {
			for (int j=0; j<224; j++) {
				unsigned char pixel = framebuffer[i/8 + j*(256/8)];
				//check each pixel value
				for (int p=0; p<8; p++) {
					if (pixel & (1<<p)) {
						SDL_RenderDrawPoint(renderer, i+p, j);
					}
				}
			}
		}

		SDL_RenderPresent(renderer);
	}

	return 0;
}
