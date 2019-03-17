#include <iostream>
#include <cstdlib>
//#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "game.h"

namespace Shooter {
	SDL_Window *Window;
	SDL_Renderer *Renderer;
	std::unique_ptr<Timer> Time = std::make_unique<Timer>();

	Game::Game()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
			exit(EXIT_FAILURE);
		}
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
			std::cerr << "Warning: Linear texture filtering not enabled!" << std::endl;
		Shooter::Window = SDL_CreateWindow("2D shooter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, SDL_WINDOW_SHOWN);
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
		if (TTF_Init() == -1) {
			std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
			exit(EXIT_FAILURE);
		}

		// �V�[���̐ݒ�
		scenes.push(std::make_unique<GameScene>());
	}

	Game::~Game()
	{
		SDL_DestroyRenderer(Shooter::Renderer);
		SDL_DestroyWindow(Shooter::Window);
		SDL_Quit();
	}

	void Game::Run()
	{
		SDL_SetRenderDrawColor(Renderer, 0x00, 0x00, 0x00, 0x00);

		bool quit = false;
		SDL_Event e;
		Time->Start();
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

			// �X�V
			Time->Update();
			scenes.top()->Update();

			// ���ۂ̕`��
			SDL_RenderClear(Renderer);
			scenes.top()->Draw();
			SDL_RenderPresent(Renderer);

			// FPS����
			Time->Delay();
		}
	}

	/*void Game::changeScene(Scene newScene)
	{

	}*/
}