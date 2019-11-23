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
		Script(EnemyManager& enemyManager, BulletManager& enemyBulletManager, PlayerManager& playerManager, BulletManager& playerBulletManager);
		Status Run();
		void LoadFile(const std::string filename);
		void RegisterFunction(const std::string name);  // TODO: 引数を伴う関数にも対応。
	private:
		enum class SceneID {
			Title,
			StageClear,
			AllClear
		};

		EnemyManager& enemyManager;
		BulletManager& enemyBulletManager;
		PlayerManager& playerManager;
		BulletManager& playerBulletManager;
		Status status;
		sol::state lua;
		// HACK: solのコルーチンの呼び出し方のために、組で保持する必要がある。Lua側でcoroutine.createをすれば、スレッドだけでもよい？
		std::list<std::pair<sol::thread, sol::coroutine>> tasksList;  // 毎フレーム実行するコルーチンのリスト。1ステージ終了する度に全て破棄する。Luaスクリプトの内部から要素を追加する。
		std::list<std::pair<sol::thread, sol::coroutine>> staticTasksList;  // 寿命の永い（明示的に破棄しない）コルーチンのリスト。自機スクリプトなどを登録する。RegisterFunction関数を介して外部から要素を登録する。

		/// <summary>敵を生成する。</summary>
		/// <param name="id">生成する敵のID</param>
		/// <param name="posX">初期位置のx座標</param>
		/// <param name="posY">初期位置のy座標</param>
		/// <param name="speed">初期速度の大きさ（速さ）</param>
		/// <param name="angle">初期速度のx軸からの角度</param>
		/// <param name="hitPoint">敵の体力</param>
		/// <returns>生成された敵へのポインタ</returns>
		std::function<std::shared_ptr<Enemy>(const EnemyManager::EnemyID, const float, const float, const float, const float, const int)> generateEnemy =
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
		std::function<std::shared_ptr<Bullet>(const BulletManager::BulletID, const float, const float, const float, const float)> generateEnemyBullet =
		[this](const BulletManager::BulletID id, const float posX, const float posY, const float speed, const float angle) -> std::shared_ptr<Bullet> {
			auto newObject = enemyBulletManager.GenerateObject(id, Vector2{ posX, posY }).lock();
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
				return std::move(generateEnemyBullet(id, enemy->GetPosition().x, enemy->GetPosition().y, speed, angle));
			else
				return nullptr;
		};

		/// <summary>指定された位置に自弾を生成する。生成元の存在は問わない。</summary>
		/// <param name="id">生成する自弾のID</param>
		/// <param name="posX">初期位置のx座標</param>
		/// <param name="posY">初期位置のy座標</param>
		/// <param name="speed">初期速度の大きさ（速さ）</param>
		/// <param name="angle">初期速度のx軸からの角度</param>
		/// <returns>生成された自弾へのポインタ</returns>
		std::function<std::shared_ptr<Bullet>(const BulletManager::ShotID, const float, const float, const float, const float)> generatePlayerBullet =
		[this](const BulletManager::ShotID id, const float posX, const float posY, const float speed, const float angle) -> std::shared_ptr<Bullet> {
			auto newObject = playerBulletManager.GenerateObject(id, Vector2{ posX, posY }).lock();
			newObject->Shot(speed, angle);
			return std::move(newObject);
		};

		/// <summary>自機を生成する。</summary>
		/// <param name="id">生成する自機のID</param>
		/// <param name="posX">初期位置のx座標</param>
		/// <param name="posY">初期位置のy座標</param>
		/// <returns>生成された自機へのポインタ</returns>
		std::function<std::shared_ptr<Player>(const PlayerManager::PlayerID, const float, const float)> generatePlayer =
		[this](const PlayerManager::PlayerID id, const float posX, const float posY) -> std::shared_ptr<Player> {
			auto newObject = playerManager.GenerateObject(id, Vector2{ posX, posY }).lock();
			newObject->Spawned();
			return std::move(newObject);
		};

		/// <summary>画面を遷移する。</summary>
		/// <param name="id">遷移先の画面ID</param>
		std::function<void(SceneID)> changeScene =
		[this](SceneID id) {
			tasksList.clear();
			for (auto&& enemy : enemyManager.GetObjects())
				enemy.lock()->Erase();
			for (auto&& bullet : enemyBulletManager.GetObjects())
				bullet.lock()->Erase();
			for (auto&& shot : playerBulletManager.GetObjects())
				shot.lock()->Erase();
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

		// TODO: 停めるための関数？
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
