#include "game.h"
#include "media.h"

namespace Shooter {
	Game::Game()
	{
		// グローバル変数（シングルトン）の初期化。SDLを初期化するために、真っ先にMediaクラスを呼ぶことに注意。
		Media::Create();
		Input::Create();
		Timer::Create();
		AssetLoader::Create();

		// シーンの設定。
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
		Media::Destroy();
	}

	void Game::Run()
	{
		Timer::Create().Start();
		while (!quitFlag) {
			// 更新
			quitFlag = Input::Create().Update();
			Timer::Create().Update();
			scenes.top()->Update();
			Timer::Create().AdjustFPS([this]() {
				// 実際の描画
				Media::Create().Clear();
				scenes.top()->Draw();
				Media::Create().Present();
			});
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
