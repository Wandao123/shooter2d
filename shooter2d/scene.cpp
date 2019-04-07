#include <iostream>
#include "game.h"
#include "user_interface.h"

using namespace Shooter;

GameScene::GameScene()
{
	// ゲーム中に常に表示されるオブジェクトを準備。
	objectsList.push_back(std::make_unique<FrameUI>(0, Game::Height - 14));
	objectsList.push_back(std::make_unique<Player>(Vector2{ (Game::Width - Player::Width) / 2.0f, Game::Height - Player::Height * 1.5f }, 4.0f, 2.0f));

	// Luaの初期化。
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::math, sol::lib::io, sol::lib::string);
	lua.script_file("scripts/stage.lua");  // TODO: エラー処理
	lua["ScreenWidth"] = Game::Width;
	lua["ScreenHeight"] = Game::Height;

	// Luaで使う関数群の登録。
	lua.new_usertype<Mover>(
		"Enemy",
		// Lua側で生成するならば、コンストラクタを定義して ``Enemy.new(...)'' とする。
		"SetSpeed", &Mover::SetSpeed,
		"GetSpeed", &Mover::GetSpeed,
		"SetAngle", &Mover::SetAngle,
		"GetAngle", &Mover::GetAngle
	);
	lua["GenerateMover"] = generateMover;
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
	for (auto&& object : objectsList)
		object->Update();
}

void GameScene::Draw()
{
	for (auto&& object : objectsList)
		object->Draw();
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
