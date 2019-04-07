#ifndef SCENE_H
#define SCENE_H

#include <list>
#include <sol.hpp>
#include <functional>
#include "game_object.h"

namespace Shooter {
	class Scene
	{
	public:
		Scene() = default;
		virtual ~Scene() = default;
		virtual void Draw() = 0;
		virtual void Update() = 0;
	};

	class GameScene : public Scene
	{
	public:
		GameScene();
		void Draw() override;
		void Update() override;
	private:
		std::list<std::shared_ptr<GameObject>> objectsList;
		sol::state lua;
		sol::thread th;    // ステージを記述するコルーチン用。
		sol::coroutine co;
		void run();
	};
}

#endif // !SCENE_H
