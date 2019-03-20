#include <iostream>
#include "game.h"
#include "user_interface.h"
#include "mover.h"

using namespace Shooter;

std::unique_ptr<std::list<std::shared_ptr<GameObject>>> PTasksList;

/******** Luaで使う関数群 ********/
int GenerateEnemy(lua_State *l)
{
	auto obj = std::make_unique<Enemy>(Vector2{ 100.0f, 0.0f }, Vector2{ 0.0f, 1.0f });
	lua_pushlightuserdata(l, static_cast<void*>(obj.get()));
	return 0;
}
/*********************************/

GameScene::GameScene()
	: state({ luaL_newstate(), [](lua_State* l) { lua_close(l); } })
	, rawState(state.get())
	, thread(lua_newthread(rawState))
{
	// ゲーム中に常に表示されるオブジェクトを準備。
	tasksList.push_back(std::make_unique<FrameUI>(0, Game::Height - 14));
	tasksList.push_back(std::make_unique<Player>(Vector2{ (Game::Width - Player::Width) / 2.0f, Game::Height - Player::Height * 1.5f }, 4.0f, 2.0f));

	// Luaの初期化。
	luaopen_base(rawState);
	if (luaL_dofile(rawState, "scripts/stage.lua")) {
		std::cerr << lua_tostring(rawState, lua_gettop(rawState)) << std::endl;
		return;
	}
	lua_getglobal(thread, "StartStage");

	// Luaで使う関数群の登録。
	PTasksList.reset(&tasksList);
	lua_register(rawState, "GenerateEnemy", [](lua_State* l) -> int {
			//generateEnemy(SmallEnemyBlue, { 100.0f, 0.0f }, { 0.0f, 1.0f });
			auto newEnemy = new Enemy(Vector2{ 100.0f, 0.0f }, Vector2{ 0.0f, 1.0f });
			lua_pushlightuserdata(l, static_cast<void*>(newEnemy));
			PTasksList->push_back(std::make_shared<Enemy>(Vector2{ 100.0f, 0.0f }, Vector2{ 0.0f, 1.0f }));
			return 1;
	});
	//lua_register(rawState, "GenerateEnemy", std::bind(foo, this, std::placeholders::_1));
	/*lua_register(rawState, "RegistrateMover", [](lua_State* l) -> int {
			auto* target = static_cast<Mover*>(lua_touserdata(l, 1));
			return 0;
	});*/
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
	/*static auto enemy = std::make_shared<Enemy>(Vector2{ (Game::Width - Player::Width) / 2.0f, 0.0f }, Vector2{ 0.0f, 1.0f });
	if (frames == 1) {
		tasksList.push_back(enemy);
	} else if (frames >= 2) {
		enemy->SetVelocity({ 4.0f * std::cos(frames * Time->GetDeltaTime() * 2.0f), 1.0f });
	}*/
	lua_resume(thread, 0);
}

int GameScene::generateEnemy(EnemiesID id, Vector2 position, Vector2 velocity)
{
	std::unique_ptr<Mover> newEnemy;
	switch (id) {
		case SmallEnemyBlue: newEnemy = std::make_unique<Enemy>(position, velocity); break;
	}
	tasksList.push_back(std::move(newEnemy));
	return 0;
}
