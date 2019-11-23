#include <algorithm>
#include "game.h"
#include "scene.h"  // ここで script.h は読み込み済み。

using namespace Shooter;

/// <param name="enemyManager">GameSceneクラスのenemyManagerメンバ変数</param>
/// <param name="enemyBulletManager">GameSceneクラスのenemyBulletManagerメンバ変数</param>
/// <param name="playerManager">GameSceneクラスのplayerManagerメンバ変数</param>
/// <param name="playerBulletManager">GameSceneクラスのplayerBulletManagerメンバ変数</param>
Script::Script(EnemyManager& enemyManager, BulletManager& enemyBulletManager, PlayerManager& playerManager, BulletManager& playerBulletManager)
	: enemyManager(enemyManager)
	, enemyBulletManager(enemyBulletManager)
	, playerManager(playerManager)
	, playerBulletManager(playerBulletManager)
	, status(Status::Running)
{
	// Luaの初期化。
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::math, sol::lib::io, sol::lib::string, sol::lib::os);

	// 各種クラスの定義
	lua.new_usertype<Bullet>(
		"Bullet",
		// Lua側で生成するならば、コンストラクタを定義して ``Enemy.new(...)'' とする。
		"Angle", sol::property(&Bullet::GetAngle, &Bullet::SetAngle),
		"PosX", sol::property([](Bullet& bullet) -> float { return bullet.GetPosition().x; }),
		"PosY", sol::property([](Bullet& bullet) -> float { return bullet.GetPosition().y; }),
		"Speed", sol::property(&Bullet::GetSpeed, &Bullet::SetSpeed),
		"Erase", &Bullet::Erase,
		"IsEnabled", &Bullet::IsEnabled,
		"TurnInvincible", &Bullet::TurnInvincible
	);
	lua.new_usertype<Enemy>(
		"Enemy",
		"Angle", sol::property(&Enemy::GetAngle, &Enemy::SetAngle),
		"HitPoint", sol::property([](Enemy& enemy) -> int { return enemy.GetHitPoint(); }),
		"PosX", sol::property([](Enemy& enemy) -> float { return enemy.GetPosition().x; }),
		"PosY", sol::property([](Enemy& enemy) -> float { return enemy.GetPosition().y; }),
		"Speed", sol::property(&Enemy::GetSpeed, &Enemy::SetSpeed),
		"Erase", &Enemy::Erase,
		"IsEnabled", &Enemy::IsEnabled,
		"TurnInvincible", &Enemy::TurnInvincible
	);
	lua.new_usertype<Player>(
		"Player",
		"Angle", sol::property(&Player::GetAngle),
		"Life", sol::property([](Player& player) -> int { return player.GetLife(); }),
		"PosX", sol::property(
			[](Player& player) -> float { return player.GetPosition().x; },
			[](Player& player, const float posX) { player.SetPosition({ posX, player.GetPosition().y }); }),
		"PosY", sol::property(
			[](Player& player) -> float { return player.GetPosition().y; },
			[](Player& player, const float posY) { player.SetPosition({ player.GetPosition().x, posY }); }),
		"Speed", sol::property(&Player::GetSpeed),
		"IsEnabled", &Player::IsEnabled,
		"SetVelocity", [](Player& player, const float dirX, const float dirY, const bool slowMode) { player.SetVelocity({ dirX, dirY }, slowMode); },
		"Spawned", &Player::Spawned,
		"TurnInvincible", &Player::TurnInvincible
	);

	// 定数の登録。
	int width = Game::Width, height = Game::Height;  // 直に代入するとgccでコンパイルできない。
	lua["ScreenWidth"] = width;
	lua["ScreenHeight"] = height;
	width = Player::Width; height = Player::Height;
	lua["PlayerWidth"] = width;
	lua["PlayerHeight"] = height;
	lua.new_enum(
		"EnemyID",
		"SmallRed", EnemyManager::EnemyID::SmallRed,
		"SmallBlue", EnemyManager::EnemyID::SmallBlue
	);
	lua.new_enum(
		"BulletID",  // 敵弾。
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
		"PlayerID",
		"Reimu", PlayerManager::PlayerID::Reimu,
		"Marisa", PlayerManager::PlayerID::Marisa,
		"Sanae", PlayerManager::PlayerID::Sanae
	);
	lua.new_enum(
		"ShotID",  // 自弾。
		"ReimuNormal", BulletManager::ShotID::ReimuNormal,
		"MarisaNormal", BulletManager::ShotID::MarisaNormal,
		"SanaeNormal", BulletManager::ShotID::SanaeNormal
	);
	lua.new_enum(
		"SceneID",
		"Title", SceneID::Title,
		"StageClear", SceneID::StageClear,
		"AllClear", SceneID::AllClear
	);
	lua.new_enum(
		"CommandID",
		"Shot", Input::Commands::Shot,
		"Bomb", Input::Commands::Bomb,
		"Slow", Input::Commands::Slow,
		"Skip", Input::Commands::Skip,
		"Leftward", Input::Commands::Leftward,
		"Rightward", Input::Commands::Rightward,
		"Forward", Input::Commands::Forward,
		"Backward", Input::Commands::Backward,
		"Pause", Input::Commands::Pause
	);

	// 関数群の登録。
	lua["GenerateBullet"] = sol::overload(
		generateEnemyBullet,
		generateBulletFromEnemy
	);
	lua["GenerateEnemy"] = generateEnemy;
	lua["GeneratePlayer"] = generatePlayer;
	lua["GenerateShot"] = generatePlayerBullet;
	lua["ChangeScene"] = changeScene;
	lua["StartCoroutine"] = sol::overload(
		startCoroutine,
		startCoroutineWithArgs
	);
	lua["GetPlayer"] = getPlayer;
	lua["GetKey"] = [](const Input::Commands command) -> bool { return Input::Create().GetKey(command); };
	lua["GetKeyDown"] = [](const Input::Commands command) -> bool { return Input::Create().GetKeyDown(command); };
	lua["GetKeyUp"] = [](const Input::Commands command) -> bool { return Input::Create().GetKeyUp(command); };
}

/// <summary>プレイ中の敵の出現や弾の生成などの全容を記述する。</summary>
/// <returns>
///	スクリプトの状態。動作中はRunning、停止中（および停止した瞬間）はDeadを返す。動作中で特にステージをクリアした際にはStageClearを返す。
/// </returns>
Script::Status Script::Run()
{
	// Luaのスレッドを実行。
	if (staticTasksList.empty() && tasksList.empty())
		status = Status::Dead;  // 自機スクリプトの実装方法との関係上、基本的にここは実行されない。
	else
		status = Status::Running;
	staticTasksList.remove_if([](std::pair<sol::thread, sol::coroutine> task) {  // 終了したスレッドを全て削除。削除しなければ再実行されることに注意。
		if (task.first.status() == sol::thread_status::dead) {
			task.first.state().collect_garbage();  // タイミングがよく判らないため、明示的に実行。もしスレッドが終了していれば、ObjectManager::objectsListの各要素の参照カウントが1になる筈。
			return true;
		} else {
			return false;
		}
	});
	for (auto&& task : staticTasksList)
		task.second();
	tasksList.remove_if([](std::pair<sol::thread, sol::coroutine> task) {
		if (task.first.status() == sol::thread_status::dead) {
			task.first.state().collect_garbage();
			return true;
		} else {
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
	}
	return status*/
}

/// <summary>スクリプトのファイルを読み込む。</summary>
/// <param name="filename">スクリプトのファイルへのパス</param>
void Script::LoadFile(const std::string filename)
{
	lua.script_file(filename);  // TODO: エラー処理、二重読み込み？
}

/// <summary>寿命の永いコルーチンを登録する。</summary>
/// <param name="name">登録する関数</param>
void Script::RegisterFunction(const std::string name)
{
	sol::thread th = sol::thread::create(lua.lua_state());
	sol::coroutine co = th.state()[name];
	co();
	staticTasksList.push_back(std::make_pair(th, co));
}