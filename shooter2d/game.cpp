#include "game.h"
#include <iostream>
#include <cstdlib>

namespace Shooter {
	SDL_Window *Window;
	SDL_Renderer *Renderer;

	Game::Game()
	{
		//SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
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

		// グローバル変数（シングルトン）の初期化
		Input::Create();
		Timer::Create();
		AssetLoader::Create();

		// シーンの設定
		PushScene(std::make_unique<TitleScene>(*this));
	}

	Game::~Game()
	{
		// SDLに依存する全ての変数を破棄してからでないと、以下の終了処理で実行時エラーが生じる（順番に注意）。
		while (!scenes.empty())
			scenes.pop();
		AssetLoader::Destroy();
		Timer::Destroy();
		Input::Destroy();
		SDL_DestroyRenderer(Shooter::Renderer);
		SDL_DestroyWindow(Shooter::Window);
		SDL_Quit();
	}

	void Game::Run()
	{
		float interval = 1000.0f / 60.0f;
		float nextFrameTime = SDL_GetTicks() + interval;
		Timer::Create().Start();
		while (!quitFlag) {
			Uint32 currentTime = SDL_GetTicks();
			
			// 更新
			quitFlag = Input::Create().Update();
			Timer::Create().Update();
			scenes.top()->Update();
			if (currentTime < nextFrameTime) {
				// 実際の描画
				SDL_SetRenderDrawColor(Renderer, 0x00, 0x00, 0x00, 0x00);
				SDL_RenderClear(Renderer);
				scenes.top()->Draw();
				SDL_RenderPresent(Renderer);

				if (currentTime < nextFrameTime) {
					// FPS制御
					//Timer::Create().Delay();  // TODO: 処理落ち対策。現在の状態ではコマが（許容できないほど）大きく飛んだりする。
					SDL_Delay(static_cast<float>(nextFrameTime - currentTime));
					interval = 1000.0f / 60.0f;
				}
			} else {
				interval *= 2;
			}
			nextFrameTime += interval;
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
