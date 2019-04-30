#include <iostream>
#include "game.h"

using namespace Shooter;

GameScene::GameScene()
{
	// 更新対象オブジェクトを生成。
	player = std::make_unique<Player>(Vector2{ Game::Width / 2.0f, Game::Height - Player::Height }, 4.0f, 2.0f);
	userInterfaceManager = std::make_unique<UserInterfaceManager>();
	userInterfaceManager->GenerateObject(UserInterfaceID::FrameRate, 0, Game::Height - 14);
	enemyManager = std::make_unique<EnemyManager>();

	// Luaの初期化。
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::math, sol::lib::io, sol::lib::string);
	lua.script_file("scripts/stage.lua");  // TODO: エラー処理

	// Luaで使う定数の登録。
	int width = Game::Width, height = Game::Height;  // 直に代入するとgccでコンパイルできない。
	lua["ScreenWidth"] = width;
	lua["ScreenHeight"] = height;
	lua["EnemyID"] = lua.create_table_with(
		"SmallBlue", EnemyID::SmallBlue
	);

	// Luaで使う関数群の登録。
	lua.new_usertype<Enemy>(
		"Enemy",
		// Lua側で生成するならば、コンストラクタを定義して ``Enemy.new(...)'' とする。
		"SetSpeed", &Enemy::SetSpeed,
		"GetSpeed", &Enemy::GetSpeed,
		"SetAngle", &Enemy::SetAngle,
		"GetAngle", &Enemy::GetAngle
	);
	lua["GenerateEnemy"] = generateEnemy;
	lua["StartCoroutine"] = sol::overload(
		startCoroutine,
		startCoroutineWithArgs
	);

	// ステージ・スクリプトの登録。
	startCoroutine("StartStage");
}

void GameScene::Update()
{
	run();
	player->Update();
	userInterfaceManager->Update();
	enemyManager->Update();
}

void GameScene::Draw()
{
	player->Draw();
	userInterfaceManager->Draw();
	enemyManager->Draw();
}

void GameScene::run()  // 処理の全容を記述
{
	for (auto task = tasksList.begin(); task != tasksList.end(); task++) {
		if (task->second.runnable())
			task->second();
		else
			task = tasksList.erase(task);
		if (tasksList.empty()) break;  // これをしないと実行時エラーが発生。
	}
}
