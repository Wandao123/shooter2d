#ifndef GAME_H
#define GAME_H

#include <memory>
#include <stack>
#include "scene.h"

namespace Shooter {
	// Gameクラスにシーンを変えることを伝えるためのインターフェース。要はscenesスタックを操作する関数群。
	class IChangingSceneListener
	{
	public:
		IChangingSceneListener() = default;
		virtual ~IChangingSceneListener() = default;
		virtual void ClearScenes() = 0;
		virtual void PushScene(std::unique_ptr<Scene>&& newScene) = 0;
		virtual void PopScene() = 0;
	};

	class Game : public IChangingSceneListener
	{
	public:
		static const int Width = 640;
		static const int Height = 480;
		Game();
		~Game();
		void Run();
		void ClearScenes() override;
		void PushScene(std::unique_ptr<Scene>&& newScene) override;
		void PopScene() override;
	private:
		std::stack<std::unique_ptr<Scene>> scenes;
	};
}

#endif // !GAME_H
