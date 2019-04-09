#ifndef SCENE_H
#define SCENE_H

#include <list>
#include <sol.hpp>
#include <functional>
#include <utility>
#include "mover.h"

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
		std::list<std::shared_ptr<GameObject>> objectsList;  // 更新対象オブジェクト。
		sol::state lua;
		// HACK: solのコルーチンの呼び出し方のために、組で保持する必要がある。Lua側でcoroutine.createをすれば、スレッドだけでもよい？
		std::list<std::pair<sol::thread, sol::coroutine>> tasksList;  // これに登録されたコルーチンが毎フレーム実行される。
		void run();
		
		// 動くオブジェクトの生成。敵や弾の種類が増えたときに面倒なので、Lua側ではなくC++側で生成する。
		std::function<std::shared_ptr<Mover>(const std::string, const float, const float, const float, const float)> generateMover =
		[this](const std::string name, const float px, const float py, const float speed, const float angle) -> std::shared_ptr<Mover> {
			std::shared_ptr<Mover> newMover;
			if (name == "SmallBlueEnemy") {
				newMover = std::make_unique<Enemy>(Vector2{ px, py }, speed, angle);
			}
			objectsList.push_back(newMover);
			return newMover;
		};

		// 本来はコルーチンを直接受け取って実行したい。しかし、スレッドを生成した上で呼び出すには、文字列で関数名を受け取る必要がある。
		std::function<void(const std::string)> startCoroutine =
		[this](const std::string name) {
			sol::thread th = sol::thread::create(lua.lua_state());
			sol::coroutine co = th.state()[name];
			co();
			tasksList.push_back(std::make_pair(th, co));
		};

		std::function<void(const std::string, sol::variadic_args)> startCoroutineWithArgs =
		[this](const std::string name, sol::variadic_args va) {
			sol::thread th = sol::thread::create(lua.lua_state());
			sol::coroutine co = th.state()[name];
			co(sol::as_args(std::vector<sol::object>(va.begin(), va.end())));
			tasksList.push_back(std::make_pair(th, co));
		};
	};
}

#endif // !SCENE_H
