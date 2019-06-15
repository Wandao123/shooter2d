#include <iostream>
#include "game.h"

using namespace Shooter;

GameScene::GameScene()
{
	// 更新対象オブジェクトを生成。
	playerManager = std::make_shared<PlayerManager>();
	playerManager->GenerateObject(PlayerManager::PlayerID::Reimu, Vector2{ Game::Width / 2.0f, Game::Height - Player::Height })->Spawned();
	userInterfaceManager = std::make_shared<UserInterfaceManager>();
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2{ 0, Game::Height - 14 });
	enemyManager = std::make_shared<EnemyManager>();
	bulletManager = std::make_shared<BulletManager>();
	effectManager = std::make_shared<EffectManager>();
	collisionDetector = std::make_unique<CollisionDetector>(playerManager, enemyManager, bulletManager, effectManager);

	// Luaの初期化。
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::math, sol::lib::io, sol::lib::string);
	lua.script_file("scripts/stage.lua");  // TODO: エラー処理

	// Luaで使う定数の登録。
	int width = Game::Width, height = Game::Height;  // 直に代入するとgccでコンパイルできない。
	lua["ScreenWidth"] = width;
	lua["ScreenHeight"] = height;
	lua["EnemyID"] = lua.create_table_with(
		"SmallBlue", EnemyManager::EnemyID::SmallBlue
	);
	lua["BulletID"] = lua.create_table_with(
		"Small", BulletManager::BulletID::Small
	);

	// Luaで使う関数群の登録。
	lua.new_usertype<Enemy>(
		"Enemy",
		// Lua側で生成するならば、コンストラクタを定義して ``Enemy.new(...)'' とする。
		"SetSpeed", &Enemy::SetSpeed,
		"GetSpeed", &Enemy::GetSpeed,
		"SetAngle", &Enemy::SetAngle,
		"GetAngle", &Enemy::GetAngle,
		"GetPosX", [](Enemy& enemy) -> float { return enemy.GetPosition().x; },
		"GetPosY", [](Enemy& enemy) -> float { return enemy.GetPosition().y; }
	);
	lua.new_usertype<Bullet>(
		"Bullet",
		"SetSpeed", &Bullet::SetSpeed,
		"GetSpeed", &Bullet::GetSpeed,
		"SetAngle", &Bullet::SetAngle,
		"GetAngle", &Bullet::GetAngle,
		"GetPosX", [](Bullet& bullet) -> float { return bullet.GetPosition().x; },
		"GetPosY", [](Bullet& bullet) -> float { return bullet.GetPosition().y; }
	);
	lua["GenerateEnemy"] = generateEnemy;
	lua["GenerateBullet"] = sol::overload(
		generateBullet,
		generateBulletFromEnemy
	);
	lua["StartCoroutine"] = sol::overload(
		startCoroutine,
		startCoroutineWithArgs
	);
	lua["GetPlayerPosX"] = [this]() -> float { return playerManager->GetPlayer()->GetPosition().x; };
	lua["GetPlayerPosY"] = [this]() -> float { return playerManager->GetPlayer()->GetPosition().y; };

	// ステージ・スクリプトの登録。
	startCoroutine("StartStage");
}

void GameScene::Update()
{
	run();
	playerManager->Update();
	userInterfaceManager->Update();
	enemyManager->Update();
	bulletManager->Update();
	effectManager->Update();
	collisionDetector->CheckAll();
}

void GameScene::Draw()
{
	playerManager->Draw();
	userInterfaceManager->Draw();
	enemyManager->Draw();
	bulletManager->Draw();
	effectManager->Draw();
}

void GameScene::run()  // 処理の全容を記述
{
	tasksList.remove_if([](std::pair<sol::thread, sol::coroutine> task) {  // 終了したスレッドを全て削除。
		if (task.first.status() == sol::thread_status::dead) {
			task.first.state().collect_garbage();  // タイミングがよく判らないため、明示的に実行。もしスレッドが終了していれば、ObjectManager::objectsListの各要素の参照カウントが1になる筈。
			return true;
		} else {
			return false;
		}
	});
	for (auto&& task : tasksList)
		task.second();
	/*int counter = Time->GetCountedFrames();
	std::vector<std::shared_ptr<Enemy>> enemies;
	if (counter % 15 == 0 && counter <= 70) {
		enemies.push_back(enemyManager->GenerateObject(EnemyManager::EnemyID::SmallBlue, Vector2{ Game::Width / 2.0f, 0.0f }));
		enemies.back()->Spawned(2.0f, M_PI_2, 100);
	}
	if (counter == 75) {
		auto newObject = effectManager->GenerateObject(EffectManager::EffectID::BlueCircle, Vector2{ Game::Width / 3.0f, 0.0f });
		newObject->Played();
	} else if (counter == 85) {
		auto newObject = effectManager->GenerateObject(EffectManager::EffectID::BlueCircle, Vector2{ Game::Width * 2.0f / 3.0f, 0.0f });
		newObject->Played();
	}*/
}