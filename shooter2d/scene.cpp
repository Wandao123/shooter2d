#include <iostream>
#include "game.h"
#include "user_interface.h"
#include "mover.h"

using namespace Shooter;

// GameSceneのメンバ変数tasksListを仲介。GameSceneが破棄されるとNULLになるため、生ポインタを使う。
// HACK: 【深刻な問題】GameSceneが生成される度に値が変わる。
std::list<std::shared_ptr<GameObject>>* PTasksList;

/******** Luaで使う関数群 ********/
int GenerateEnemy(lua_State* l)
{
	const std::string name = lua_tostring(l, 1);
	std::unique_ptr<Enemy> newEnemy;
	//auto newEnemy = static_cast<std::unique_ptr<Enemy>*>(lua_newuserdata(l, sizeof(std::unique_ptr<Enemy>)));
	//luaL_newmetatable(l, "ENEMYP");
	//lua_setmetatable(l, -1);
	if (name == "SmallBlue") {
		float px = static_cast<float>(lua_tonumber(l, 2));
		float py = static_cast<float>(lua_tonumber(l, 3));
		float vx = static_cast<float>(lua_tonumber(l, 4));
		float vy = static_cast<float>(lua_tonumber(l, 5));
		//*newEnemy = new Enemy(Vector2{ px, py }, Vector2{ vx, vy });
		newEnemy = std::make_unique<Enemy>(Vector2{ px, py }, Vector2{ vx, vy });
	}
	lua_pushlightuserdata(l, static_cast<void*>(newEnemy.get()));  // HACK: lightuserdataは非推奨？
	PTasksList->push_back(std::move(newEnemy));
	return 1;
}

int GetVelocity(lua_State* l)
{
	Mover* target = static_cast<Mover*>(lua_touserdata(l, 1));
	lua_pop(l, lua_gettop(l));
	lua_newtable(l);
	lua_pushnumber(l, target->GetVelocity().x);
	lua_setfield(l, -2, "x");
	lua_pushnumber(l, target->GetVelocity().y);
	lua_setfield(l, -2, "y");
	return 1;
}

int SetVelocity(lua_State* l)
{
	Mover* target = static_cast<Mover*>(lua_touserdata(l, 1));
	float vx = luaL_checknumber(l, 2);
	float vy = luaL_checknumber(l, 3);
	target->SetVelocity({ vx, vy });
	return 0;
}

int AddForce(lua_State* l)
{
	Mover* target = static_cast<Mover*>(lua_touserdata(l, 1));
	float fx = luaL_checknumber(l, 2);
	float fy = luaL_checknumber(l, 3);
	target->AddForce({ fx, fy });
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
	luaL_openlibs(rawState);
	if (luaL_dofile(rawState, "scripts/stage.lua")) {
		std::cerr << lua_tostring(rawState, lua_gettop(rawState)) << std::endl;
		return;
	}
	lua_pushnumber(rawState, Game::Width);
	lua_setglobal(rawState, "ScreenWidth");
	lua_pushnumber(rawState, Game::Height);
	lua_setglobal(rawState, "ScreenHeight");

	// Luaで使う関数群の登録。
	PTasksList = &tasksList;
	lua_register(rawState, "GenerateEnemy", &GenerateEnemy);
	lua_register(rawState, "GetVelocity", &GetVelocity);
	lua_register(rawState, "SetVelocity", &SetVelocity);
	lua_register(rawState, "AddForce", &AddForce);

	// Luaのコルーチンの登録。
	lua_getglobal(thread, "StartStage");
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
	lua_resume(thread, nullptr, 0);
}
