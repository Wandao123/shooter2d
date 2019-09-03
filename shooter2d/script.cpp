#include "game.h"
#include "scene.h"  // ここで script.h は読み込み済み。

using namespace Shooter;

/// <param name="bulletManager">GameSceneクラスのbulletManagerメンバ変数</param>
/// <param name="enemyManager">GameSceneクラスのenemyManagerメンバ変数</param>
/// <param name="playerManager">GameSceneクラスのplayerManagerメンバ変数</param>
Script::Script(BulletManager& bulletManager, EnemyManager& enemyManager, PlayerManager& playerManager)
	: bulletManager(bulletManager)
	, enemyManager(enemyManager)
	, playerManager(playerManager)
	, status(Status::Running)
{
	// Luaの初期化。
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::math, sol::lib::io, sol::lib::string, sol::lib::os);
	//const std::string package_path = lua["package"]["path"];
	//lua["package"]["path"] = package_path + ";./scripts/?.lua";  // Luaのpackage.pathにscriptsディレクトリを追加。
	lua.script_file(MainScript);  // TODO: エラー処理

	// 各種クラスの定義
	lua.new_usertype<Bullet>(
		"Bullet",
		// Lua側で生成するならば、コンストラクタを定義して ``Enemy.new(...)'' とする。
		"Speed", sol::property(&Bullet::GetSpeed, &Bullet::SetSpeed),
		"Angle", sol::property(&Bullet::GetAngle, &Bullet::SetAngle),
		"PosX", sol::property([](Bullet& bullet) -> float { return bullet.GetPosition().x; }),
		"PosY", sol::property([](Bullet& bullet) -> float { return bullet.GetPosition().y; }),
		"IsEnabled", &Bullet::IsEnabled,
		"Erase", &Bullet::Erase
	);
	lua.new_usertype<Enemy>(
		"Enemy",
		"Speed", sol::property(&Enemy::GetSpeed, &Enemy::SetSpeed),
		"Angle", sol::property(&Enemy::GetAngle, &Enemy::SetAngle),
		"PosX", sol::property([](Enemy& enemy) -> float { return enemy.GetPosition().x; }),
		"PosY", sol::property([](Enemy& enemy) -> float { return enemy.GetPosition().y; }),
		"Erase", &Enemy::Erase,
		"IsEnabled", &Enemy::IsEnabled,
		"HitPoint", sol::property([](Enemy& enemy) -> int { return enemy.GetHitPoint(); })
	);
	lua.new_usertype<Player>(
		"Player",
		"Speed", sol::property(&Player::GetSpeed),
		"Angle", sol::property(&Player::GetAngle),
		"PosX", sol::property([](Player& player) -> float { return player.GetPosition().x; }),
		"PosY", sol::property([](Player& player) -> float { return player.GetPosition().y; }),
		"IsEnabled", &Player::IsEnabled
	);

	// 定数の登録。
	int width = Game::Width, height = Game::Height;  // 直に代入するとgccでコンパイルできない。
	lua["ScreenWidth"] = width;
	lua["ScreenHeight"] = height;
	lua.new_enum(
		"EnemyID",
		"SmallRed", EnemyManager::EnemyID::SmallRed,
		"SmallBlue", EnemyManager::EnemyID::SmallBlue
	);
	lua.new_enum(
		"BulletID",
		"LargeRed", BulletManager::BulletID::LargeRed,
		"LargeBlue", BulletManager::BulletID::LargeBlue,
		"MiddleRed", BulletManager::BulletID::MiddleRed,
		"MiddleBlue", BulletManager::BulletID::MiddleBlue,
		"SmallRed", BulletManager::BulletID::SmallRed,
		"SmallBlue", BulletManager::BulletID::SmallBlue,
		"TinyRed", BulletManager::BulletID::TinyRed,
		"TinyBlue", BulletManager::BulletID::TinyBlue,
		"ScaleRed", BulletManager::BulletID::ScaleRed,
		"ScaleBlue", BulletManager::BulletID::ScaleBlue,
		"RiceRed", BulletManager::BulletID::RiceRed,
		"RiceBlue", BulletManager::BulletID::RiceBlue
	);
	lua.new_enum(
		"SceneID",
		"Title", SceneID::Title,
		"StageClear", SceneID::StageClear,
		"AllClear", SceneID::AllClear
	);

	// 関数群の登録。
	lua["GenerateEnemy"] = generateEnemy;
	lua["GenerateBullet"] = sol::overload(
		generateBullet,
		generateBulletFromEnemy
	);
	lua["ChangeScene"] = changeScene;
	lua["StartCoroutine"] = sol::overload(
		startCoroutine,
		startCoroutineWithArgs
	);
	lua["GetPlayer"] = getPlayer;  // 定数として扱うべき？　その場合は事前に生成されていることを要求する。

	// Main関数の登録。
	sol::thread th = sol::thread::create(lua.lua_state());
	sol::coroutine co = th.state()["Main"];
	mainTask = std::make_pair(th, co);
}

/// <summary>プレイ中の敵の出現や弾の生成などの全容を記述する。</summary>
/// <returns>
///	スクリプトの状態。動作中はRunning、停止中（および停止した瞬間）はDeadを返す。動作中で特にステージをクリアした際にはStageClearを返す。
/// </returns>
Script::Status Script::Run()
{
	// Luaのスレッドを実行。
	if (!mainTask.second.runnable() && tasksList.empty())
		status = Status::Dead;
	else
		status = Status::Running;
	mainTask.second();
	tasksList.remove_if([](std::pair<sol::thread, sol::coroutine> task) {  // 終了したスレッドを全て削除。
		if (task.first.status() == sol::thread_status::dead) {
			task.first.state().collect_garbage();  // タイミングがよく判らないため、明示的に実行。もしスレッドが終了していれば、ObjectManager::objectsListの各要素の参照カウントが1になる筈。
			return true;
		}
		else {
			return false;
		}
	});
	for (auto&& task : tasksList)
		task.second();
	return status;

	// Luaを使わない場合は、以下のように記述する。
	/*int counter = Timer::Create().GetPlayingFrames();
	std::vector<std::shared_ptr<Enemy>> enemies;
	if (counter % 15 == 0 && counter <= 70) {
		enemies.push_back(enemyManager->GenerateObject(EnemyManager::EnemyID::SmallBlue, Vector2{ Game::Width / 2.0f, 0.0f }));
		enemies.back()->Spawned(2.0f, M_PI_2, 100);
	}*/
}