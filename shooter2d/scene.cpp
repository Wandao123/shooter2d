#include <iostream>
#include "game.h"
#include "user_interface.h"
#include "mover.h"

using namespace Shooter;

// GameSceneのメンバ変数tasksListを仲介。GameSceneが破棄されるとNULLになるため、生ポインタを使う。
// HACK: 【深刻な問題】GameSceneが生成される度に指し示す実体が変わる。
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
		float px = static_cast<float>(luaL_checknumber(l, 2));
		float py = static_cast<float>(luaL_checknumber(l, 3));
		float speed = static_cast<float>(luaL_checknumber(l, 4));
		float angle = static_cast<float>(luaL_checknumber(l, 5));
		//*newEnemy = new Enemy(Vector2{ px, py }, Vector2{ vx, vy });
		newEnemy = std::make_unique<Enemy>(Vector2{ px, py }, speed, angle);
	}
	lua_pushlightuserdata(l, static_cast<void*>(newEnemy.get()));  // HACK: lightuserdataは非推奨？
	PTasksList->push_back(std::move(newEnemy));
	return 1;
}

// HACK: Setterとgetterはラムダ式でまとめて書けないか？
int GetSpeed(lua_State* l)
{
	Mover* target = static_cast<Mover*>(lua_touserdata(l, 1));
	lua_pop(l, lua_gettop(l));
	lua_pushnumber(l, target->GetSpeed());
	return 1;
}

int SetSpeed(lua_State* l)
{
	Mover* target = static_cast<Mover*>(lua_touserdata(l, 1));
	float angle = static_cast<float>(luaL_checknumber(l, 2));
	target->SetAngle(angle);
	return 0;
}

int GetAngle(lua_State* l)
{
	Mover* target = static_cast<Mover*>(lua_touserdata(l, 1));
	lua_pop(l, lua_gettop(l));
	lua_pushnumber(l, target->GetAngle());
	return 1;
}

int SetAngle(lua_State* l)
{
	Mover* target = static_cast<Mover*>(lua_touserdata(l, 1));
	float angle = static_cast<float>(luaL_checknumber(l, 2));
	target->SetAngle(angle);
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
	lua_pushinteger(rawState, Game::Width);
	lua_setglobal(rawState, "ScreenWidth");
	lua_pushinteger(rawState, Game::Height);
	lua_setglobal(rawState, "ScreenHeight");

	// Luaで使う関数群の登録。
	PTasksList = &tasksList;
	lua_register(rawState, "GenerateEnemy", &GenerateEnemy);
	lua_register(rawState, "GetSpeed", &GetSpeed);
	lua_register(rawState, "SetSpeed", &SetSpeed);
	lua_register(rawState, "GetAngle", &GetAngle);
	lua_register(rawState, "SetAngle", &SetAngle);

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
	/*static Uint32 frames = 0;
	static auto enemy = std::make_shared<Enemy>(Vector2{ (Game::Width - Player::Width) / 2.0f, 0.0f }, 3.0f, M_PI_2);
	++frames;
	if (frames == 1) {
		tasksList.push_back(enemy);
	} else if (frames >= 2) {
		enemy->SetAngle(frames * Time->GetDeltaTime() * 2.0f);
	}*/
	static int status = LUA_YIELD;
	if (status == LUA_YIELD)  // コルーチンが終了したら実行を停める。
		status = lua_resume(thread, nullptr, 0);
}
