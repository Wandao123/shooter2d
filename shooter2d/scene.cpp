#include <iostream>
#include "game.h"
#include "user_interface.h"
#include "mover.h"

using namespace Shooter;

GameScene::GameScene()
{
	// ゲーム中に常に表示されるオブジェクトを準備。
	tasksList.push_back(std::make_unique<FrameUI>(0, Game::Height - 14));
	tasksList.push_back(std::make_unique<Player>(Vector2{ (Game::Width - Player::Width) / 2.0f, Game::Height - Player::Height * 1.5f }, 4.0f, 2.0f));

	// Luaの初期化。
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::math, sol::lib::io, sol::lib::string);
	lua.script_file("scripts/stage.lua");  // TODO: エラー処理
	lua["ScreenWidth"] = Game::Width;
	lua["ScreenHeight"] = Game::Height;

	// Luaで使う関数群の登録。
	lua.new_usertype<Enemy>(
		"Enemy",
		// Lua側で生成するならば、コンストラクタを定義して ``Enemy.new(...)'' とする。
		"SetSpeed", &Enemy::SetSpeed,
		"GetSpeed", &Enemy::GetSpeed,
		"SetAngle", &Enemy::SetAngle,
		"GetAngle", &Enemy::GetAngle
	);
	// 敵の種類が増えたときに面倒なので、Lua側ではなくC++側で生成する。
	lua["GenerateEnemy"] = [this](const std::string name, const float px, const float py, const float speed, const float angle) -> std::shared_ptr<Enemy> {
		std::shared_ptr<Enemy> newEnemy;
		if (name == "SmallBlue") {
			newEnemy = std::make_unique<Enemy>(Vector2{ px, py }, speed, angle);
		}
		tasksList.push_back(newEnemy);
		return newEnemy;
	};

	// Luaのコルーチンの登録。
	th = sol::thread::create(lua.lua_state());
	co = th.state()["StartStage"];
}

void GameScene::Update()
{
	run();
	for (auto&& task : tasksList)
		task->Update();
}

void GameScene::Draw()
{
	for (auto&& task : tasksList)
		task->Draw();
}

void GameScene::run()  // 処理の全容を記述
{
	if (co.runnable())
		co();
}
