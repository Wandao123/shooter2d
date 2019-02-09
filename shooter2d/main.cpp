/**
  date: 2019/01/26
*/
#include <iostream>
//#include <memory>
#include <cstdlib>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "user_interface.h"
#include "object.h"

namespace Shooter {
	SDL_Window *Window;
	SDL_Renderer *Renderer;
	TTF_Font *Font;  // HACK: フォントはグローバル変数で定義する必要がある？
	std::unique_ptr<Timer> Time;
}

void Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		std::cerr << "Warning: Linear texture filtering not enabled!" << std::endl;
	Shooter::Window = SDL_CreateWindow("2D shooter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
	if (Shooter::Window == nullptr) {
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	//Shooter::renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	Shooter::Renderer = SDL_CreateRenderer(Shooter::Window, -1, SDL_RENDERER_ACCELERATED);
	if (Shooter::Renderer == nullptr) {
		std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	if (TTF_Init() == -1) {
		std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	Shooter::Font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 14);
	if (Shooter::Font == nullptr) {
		std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
	}
}

void Finalize()
{
	TTF_CloseFont(Shooter::Font);
	SDL_DestroyRenderer(Shooter::Renderer);
	SDL_DestroyWindow(Shooter::Window);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	Initialize();
	Shooter::Time.reset(new Shooter::Timer);
	SDL_SetRenderDrawColor(Shooter::Renderer, 0x00, 0x00, 0x00, 0x00);
	
	std::unique_ptr<Shooter::Task> frameUI(new Shooter::FrameUI(0, ScreenHeight - 14));
	std::unique_ptr<Shooter::Task> player(new Shooter::Player("images/Reimudot.png", 4.0f, 2.0f));

	bool quit = false;
	SDL_Event e;
	Shooter::Time->Start();
	while (!quit) {
		while (SDL_PollEvent(&e) != 0) {
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = true;
				break;
			}
		}

		// 更新
		Shooter::Time->Update();
		frameUI->Update();
		player->Update();

		// 実際の描画
		SDL_RenderClear(Shooter::Renderer);
		frameUI->Draw();
		player->Draw();
		SDL_RenderPresent(Shooter::Renderer);

		Shooter::Time->Delay();
	}

	Finalize();
	return 0;
}