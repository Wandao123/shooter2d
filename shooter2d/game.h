#ifndef GAME_H
#define GAME_H

#include <stack>
#include "scene.h"

namespace Shooter {
	/// <summary>SceneクラスからGameクラスにシーンの変更を伝えるためのインターフェース。要はscenesスタックを操作する関数群。</summary>
	class IChangingSceneListener
	{
	public:
		IChangingSceneListener() = default;
		virtual ~IChangingSceneListener() = default;
		virtual void ClearAndChangeScene(std::unique_ptr<Scene>&& newScene) = 0;
		virtual void PushScene(std::unique_ptr<Scene>&& newScene) = 0;
		virtual void PopScene() = 0;
		virtual void Quit() = 0;
	};

	/// <summary>ゲームループを記述するクラス。</summary>
	/// <remarks>基本的に各シーンを表示することのみ担当する。シーンの切り替え先の指定は各Sceneクラスに任せる。</remarks>
	class Game : public IChangingSceneListener
	{
	public:
		Game();
		~Game();
		void Run();
		void ClearAndChangeScene(std::unique_ptr<Scene>&& newScene) override;
		void PushScene(std::unique_ptr<Scene>&& newScene) override;
		void PopScene() override;
		void Quit() override;
	private:
		bool quitFlag = false;
		std::stack<std::unique_ptr<Scene>> scenes;
	};
}

#endif // !GAME_H
