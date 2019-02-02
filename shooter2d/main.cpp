/**
  date: 2019/01/26
*/
#include <iostream>
#include <string>
#include <cstdlib>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

const int ScreenWidth = 640;
const int ScreenHeight = 480;
const int ScreenFPS = 60;
const int ScreenTicksPerFrame = 1000 / ScreenFPS;

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		std::cerr << "Warning: Linear texture filtering not enabled!" << std::endl;
	SDL_Window *window = SDL_CreateWindow("2D shooter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	std::string path("images/Reimudot.png");
	SDL_Texture *texture = IMG_LoadTexture(renderer, path.c_str());
	if (texture == nullptr) {
		std::cerr << "Unable to load image " << path << "!  SDL_image Error: " << IMG_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_Rect clips[5] = {
		{ 0, 0, 32, 48 },
		{32, 0, 32, 48},
		{64, 0, 32, 48},
		{96, 0, 32, 48},
		{128, 0, 32, 48}
	};
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	bool quit = false;
	SDL_Event e;
	unsigned frame = 0;
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_q:
					quit = true;
					break;
				}
			}
		}

		SDL_RenderClear(renderer);
		SDL_Rect *currentClip = &clips[(frame / 6) % 5];
		SDL_Rect renderQuad = { (ScreenWidth - 32) / 2, (ScreenHeight - 48) / 2, 32, 48 };
		SDL_RenderCopy(renderer, texture, currentClip, &renderQuad);
		SDL_RenderPresent(renderer);
		frame = ++frame % 300;
	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
	return 0;
}