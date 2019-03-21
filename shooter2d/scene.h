#ifndef SCENE_H
#define SCENE_H

#include <list>
#include <lua.hpp>
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
		std::list<std::shared_ptr<GameObject>> tasksList;
		std::unique_ptr<lua_State, std::function<void(lua_State*)>> state;  // HACK: あまりfunctionを使わない方が良い？
		lua_State* const rawState;  // stateの生ポインタ。
		lua_State* const thread;    // ステージを記述するコルーチン用。
		void run();
	};
}

#endif // !SCENE_H
