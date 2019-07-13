#include <iostream>
#include <cstdlib>
#include "game.h"

namespace Shooter {
	SDL_Window *Window;
	SDL_Renderer *Renderer;
	std::unique_ptr<Timer> Time;
	std::unique_ptr<CAssetLoader> AssetLoader;

	Game::Game()
	{
		//SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
			exit(EXIT_FAILURE);
		}
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
			std::cerr << "Warning: Linear texture filtering not enabled!" << std::endl;
		Window = SDL_CreateWindow("Bullet Hell 2D Shmup", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width, Height, SDL_WINDOW_SHOWN);
		if (Shooter::Window == nullptr) {
			std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
			exit(EXIT_FAILURE);
		}
		Renderer = SDL_CreateRenderer(Shooter::Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		//Renderer = SDL_CreateRenderer(Shooter::Window, -1, SDL_RENDERER_ACCELERATED);
		if (Shooter::Renderer == nullptr) {
			std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
			exit(EXIT_FAILURE);
		}

		// グローバル変数の初期化
		Time = std::make_unique<Timer>();
		AssetLoader = std::make_unique<CAssetLoader>();

		// シーンの設定
		PushScene(std::make_unique<TitleScene>(*this));
	}

	Game::~Game()
	{
		// SDLに依存する全ての変数を破棄してからでないと、以下の終了処理で実行時エラーが生じる（順番に注意）。
		while (!scenes.empty())
			scenes.pop();
		AssetLoader.reset();
		Time.reset();
		SDL_DestroyRenderer(Shooter::Renderer);
		SDL_DestroyWindow(Shooter::Window);
		SDL_Quit();
	}

	void Game::Run()
	{
		SDL_SetRenderDrawColor(Renderer, 0x00, 0x00, 0x00, 0x00);

		SDL_Event e;
		Time->Start();
		while (!quitFlag) {
			while (SDL_PollEvent(&e) != 0) {
				switch (e.type) {
				case SDL_QUIT:
					quitFlag = true;
					break;
				case SDL_KEYDOWN:
					if (e.key.keysym.sym == SDLK_ESCAPE)
						quitFlag = true;
					break;
				}
			}

			// 更新
			Time->Update();
			scenes.top()->Update();

			// 実際の描画
			SDL_RenderClear(Renderer);
			scenes.top()->Draw();
			SDL_RenderPresent(Renderer);

			// FPS制御
			//Time->Delay();
		}
	}

	void Game::ClearAndChangeScene(std::unique_ptr<Scene>&& newScene)
	{
		while (!scenes.empty())
			scenes.pop();  // 呼び出し元が破棄されるから危険？
		scenes.push(std::move(newScene));
	}

	void Game::PushScene(std::unique_ptr<Scene>&& newScene)
	{
		scenes.push(std::move(newScene));
	}

	void Game::PopScene()
	{
		if (scenes.size() > 1)
			scenes.pop();
	}

	void Game::Quit()
	{
		quitFlag = true;
	}
}
