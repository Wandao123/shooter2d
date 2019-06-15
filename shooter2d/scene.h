﻿#ifndef SCENE_H
#define SCENE_H

#include <list>
#include <sol.hpp>
#include <functional>
#include <utility>
#include "collision_detector.h"
#include "user_interface.h"

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
		std::shared_ptr<PlayerManager> playerManager;
		std::shared_ptr<Player> player;
		std::shared_ptr<UserInterfaceManager> userInterfaceManager;
		std::shared_ptr<EnemyManager> enemyManager;
		std::shared_ptr<BulletManager> bulletManager;
		std::shared_ptr<EffectManager> effectManager;
		std::unique_ptr<CollisionDetector> collisionDetector;
		sol::state lua;
		// HACK: solのコルーチンの呼び出し方のために、組で保持する必要がある。Lua側でcoroutine.createをすれば、スレッドだけでもよい？
		std::list<std::pair<sol::thread, sol::coroutine>> tasksList;  // これに登録されたコルーチンが毎フレーム実行される。
		void run();

		// 敵の生成。種類が増えたときに面倒なので、Lua側ではなくC++側で生成する。
		std::function<std::shared_ptr<Enemy>(const EnemyManager::EnemyID, const float, const float, const float, const float, const int hitPoint)> generateEnemy =
		[this](const EnemyManager::EnemyID id, const float posX, const float posY, const float speed, const float angle, const int hitPoint) -> std::shared_ptr<Enemy> {
			auto newObject = enemyManager->GenerateObject(id, Vector2{ posX, posY });
			newObject->Spawned(speed, angle, hitPoint);
			return std::move(newObject);
		};

		// 指定された位置に敵弾を生成する。生成元の存在は問わない。
		std::function<std::shared_ptr<Bullet>(const BulletManager::BulletID, const float, const float, const float, const float)> generateBullet =
		[this](const BulletManager::BulletID id, const float posX, const float posY, const float speed, const float angle) -> std::shared_ptr<Bullet> {
			auto newObject = bulletManager->GenerateObject(id, Vector2{ posX, posY });
			newObject->Shot(speed, angle);
			return std::move(newObject);
		};

		// 指定した敵から敵弾を生成する。生成元が存在しなければ生成しない。
		std::function<std::shared_ptr<Bullet>(const BulletManager::BulletID, std::shared_ptr<Enemy>, const float, const float)> generateBulletFromEnemy =
		[this](const BulletManager::BulletID id, std::shared_ptr<Enemy> enemy, const float speed, const float angle) -> std::shared_ptr<Bullet> {
			if (enemy->IsEnabled())
				return std::move(generateBullet(id, enemy->GetPosition().x, enemy->GetPosition().y, speed, angle));
			else
				return nullptr;
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
