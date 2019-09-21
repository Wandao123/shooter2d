#ifndef SCRIPT_H
#define SCRIPT_H

#include <list>
#include <functional>
#include "mover.h"

#ifdef _WIN64
#include <sol/sol.hpp>
#elif __linux__
#include <sol.hpp>
#endif

namespace Shooter {
	// GameSceneクラスのみから呼び出されると想定。
	class Script
	{
	public:
		enum class Status {
			AllClear,
			Dead,
			Running,
			StageClear
		};

		// スマート・ポインタではなくとも参照で十分なときには参照をつかう。
		Script(BulletManager& bulletManager, EnemyManager& enemyManager, PlayerManager& playerManager);
		Status Run();
	private:
		enum class SceneID {
			Title,
			StageClear,
			AllClear
		};

		const std::string MainScript = "scripts/main.lua";
		BulletManager& bulletManager;
		EnemyManager& enemyManager;
		PlayerManager& playerManager;
		Status status;
		sol::state lua;
		// HACK: solのコルーチンの呼び出し方のために、組で保持する必要がある。Lua側でcoroutine.createをすれば、スレッドだけでもよい？
		std::list<std::pair<sol::thread, sol::coroutine>> tasksList;  // これに登録されたコルーチンが毎フレーム実行される。
		std::pair<sol::thread, sol::coroutine> mainTask;  // Main関数用の特別なスレッド。

		/// <summary>敵を生成する。</summary>
		/// <param name="id">生成する敵のID</param>
		/// <param name="posX">初期位置のx座標</param>
		/// <param name="posY">初期位置のy座標</param>
		/// <param name="speed">初期速度の大きさ（速さ）</param>
		/// <param name="angle">初期速度のx軸からの角度</param>
		/// <param name="hitPoint">敵の体力</param>
		/// <returns>生成された敵へのポインタ</returns>
		std::function<std::shared_ptr<Enemy>(const EnemyManager::EnemyID, const float, const float, const float, const float, const int hitPoint)> generateEnemy =
		[this](const EnemyManager::EnemyID id, const float posX, const float posY, const float speed, const float angle, const int hitPoint) -> std::shared_ptr<Enemy> {
			auto newObject = enemyManager.GenerateObject(id, Vector2{ posX, posY }).lock();
			newObject->Spawned(speed, angle, hitPoint);
			return std::move(newObject);
		};

		/// <summary>指定された位置に敵弾を生成する。生成元の存在は問わない。</summary>
		/// <param name="id">生成する敵弾のID</param>
		/// <param name="posX">初期位置のx座標</param>
		/// <param name="posY">初期位置のy座標</param>
		/// <param name="speed">初期速度の大きさ（速さ）</param>
		/// <param name="angle">初期速度のx軸からの角度</param>
		/// <returns>生成された敵弾へのポインタ</returns>
		std::function<std::shared_ptr<Bullet>(const BulletManager::BulletID, const float, const float, const float, const float)> generateBullet =
		[this](const BulletManager::BulletID id, const float posX, const float posY, const float speed, const float angle) -> std::shared_ptr<Bullet> {
			auto newObject = bulletManager.GenerateObject(id, Vector2{ posX, posY }).lock();
			newObject->Shot(speed, angle);
			return std::move(newObject);
		};

		/// <summary>指定した敵から敵弾を生成する。生成元が存在しなければ生成しない。</summary>
		/// <param name="id">生成する敵弾のID</param>
		/// <param name="enemy">生成元の敵へのポインタ</param>
		/// <param name="speed">初期速度の大きさ（速さ）</param>
		/// <param name="angle">初期速度のx軸からの角度</param>
		/// <returns>生成された敵弾へのポインタ</returns>
		std::function<std::shared_ptr<Bullet>(const BulletManager::BulletID, std::shared_ptr<Enemy>, const float, const float)> generateBulletFromEnemy =
		[this](const BulletManager::BulletID id, std::shared_ptr<Enemy> enemy, const float speed, const float angle) -> std::shared_ptr<Bullet> {
			if (enemy->IsEnabled())
				return std::move(generateBullet(id, enemy->GetPosition().x, enemy->GetPosition().y, speed, angle));
			else
				return nullptr;
		};

		/// <summary>画面を遷移する。</summary>
		/// <param name="id">遷移先の画面ID</param>
		std::function<void(SceneID)> changeScene =
		[this](SceneID id) {
			tasksList.clear();
			for (auto&& enemy : enemyManager.GetObjects())
				enemy.lock()->Erase();
			for (auto&& bullet : bulletManager.GetObjects())
				bullet.lock()->Erase();
			switch (id) {
			case SceneID::Title:
				status = Status::Dead;
				break;
			case SceneID::StageClear:
				status = Status::StageClear;
				break;
			case SceneID::AllClear:
				status = Status::AllClear;
				break;
			}
		};

		/// <summary>コルーチンを登録する。登録されたコルーチンは毎フレーム実行される。</summary>
		/// <param name="func">登録するコルーチン</param>
		std::function<void(const sol::function)> startCoroutine =
		[this](const sol::function func) {
			sol::thread th = sol::thread::create(lua.lua_state());
			sol::coroutine co(th.state(), func);
			co();
			tasksList.push_back(std::make_pair(th, co));
		};

		/// <summary>コルーチンを登録する。登録されたコルーチンは毎フレーム実行される。</summary>
		/// <param name="func">登録するコルーチン</param>
		/// <param name="va">コルーチンの引数リスト</param>
		std::function<void(const sol::function, sol::variadic_args)> startCoroutineWithArgs =
		[this](const sol::function func, sol::variadic_args va) {
			sol::thread th = sol::thread::create(lua.lua_state());
			sol::coroutine co(th.state(), func);
			co(sol::as_args(std::vector<sol::object>(va.begin(), va.end())));
			tasksList.push_back(std::make_pair(th, co));
		};

		/// <summary>自機へのポインタを取得</summary>
		/// <returns>自機へのポインタ</returns>
		std::function<std::shared_ptr<Player>(void)> getPlayer =
		[this]() -> std::shared_ptr<Player> {
			auto player = playerManager.GetPlayer();
			return (player.expired()) ? nullptr : player.lock();
		};
	};
}

#endif // !SCRIPT_H
