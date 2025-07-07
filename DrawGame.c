#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

int main() {
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Event e;
	SDL_Init(SDL_INIT_VIDEO);
	bool running = true;
	SDL_Rect r = {10,10,250,250};

	SDL_CreateWindowAndRenderer(256 * 4, 224 * 4, 0, &window, &renderer); //Space invaders screen output is 256x244
	SDL_RenderSetScale(renderer, 4, 4);

	while (running) {
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT)
				running = false;
		}

		SDL_SetRenderDrawColor(renderer,0,0,0,255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer,255,255,255,255);
		SDL_RenderFillRect(renderer, &r);

		SDL_RenderPresent(renderer);
		SDL_Delay(10);
	}

	return 0;
}
