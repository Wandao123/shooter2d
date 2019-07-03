#include "game.h"
#include "scene.h"

using namespace Shooter;

/// <param name="bulletManager">GameSceneクラスのbulletManagerメンバ変数</param>
/// <param name="enemyManager">GameSceneクラスのenemyManagerメンバ変数</param>
/// <param name="playerManager">GameSceneクラスのplayerManagerメンバ変数</param>
Script::Script(BulletManager& bulletManager, EnemyManager& enemyManager, PlayerManager& playerManager)
	: bulletManager(bulletManager)
	, enemyManager(enemyManager)
	, playerManager(playerManager)
{
	// Luaの初期化。
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::math, sol::lib::io, sol::lib::string, sol::lib::os);
	lua.script_file("scripts/stage.lua");  // TODO: エラー処理

	// 各種クラスの定義
	lua.new_usertype<Bullet>(
		"Bullet",
		// Lua側で生成するならば、コンストラクタを定義して ``Enemy.new(...)'' とする。
		"SetSpeed", &Bullet::SetSpeed,
		"GetSpeed", &Bullet::GetSpeed,
		"SetAngle", &Bullet::SetAngle,
		"GetAngle", &Bullet::GetAngle,
		"GetPosX", [](Bullet& bullet) -> float { return bullet.GetPosition().x; },
		"GetPosY", [](Bullet& bullet) -> float { return bullet.GetPosition().y; }
	);
	lua.new_usertype<Enemy>(
		"Enemy",
		"SetSpeed", &Enemy::SetSpeed,
		"GetSpeed", &Enemy::GetSpeed,
		"SetAngle", &Enemy::SetAngle,
		"GetAngle", &Enemy::GetAngle,
		"GetPosX", [](Enemy& enemy) -> float { return enemy.GetPosition().x; },
		"GetPosY", [](Enemy& enemy) -> float { return enemy.GetPosition().y; }
	);
	lua.new_usertype<Player>(
		"Player",
		"GetSpeed", &Player::GetSpeed,
		"GetAngle", &Player::GetAngle,
		"GetPosX", [](Player& player) -> float { return player.GetPosition().x; },
		"GetPosY", [](Player& player) -> float { return player.GetPosition().y; }
	);

	// 定数の登録。
	int width = Game::Width, height = Game::Height;  // 直に代入するとgccでコンパイルできない。
	lua["ScreenWidth"] = width;
	lua["ScreenHeight"] = height;
	lua.new_enum(
		"EnemyID",
		"SmallBlue", EnemyManager::EnemyID::SmallBlue
	);
	lua.new_enum(
		"BulletID",
		"Small", BulletManager::BulletID::Small
	);

	// 関数群の登録。
	lua["GenerateEnemy"] = generateEnemy;
	lua["GenerateBullet"] = sol::overload(
		generateBullet,
		generateBulletFromEnemy
	);
	lua["StartCoroutine"] = sol::overload(
		startCoroutine,
		startCoroutineWithArgs
	);
	lua["GetPlayer"] = getPlayer;  // 定数として扱うべき？　その場合は事前に生成されていることを要求する。

	// ステージ・スクリプトの登録。
	startCoroutine(lua["StartStage"]);
}

/// <summary>プレイ中の敵の出現や弾の生成などの全容を記述する。</summary>
void Script::Run()
{
	// Luaのスレッドを実行。
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

	// Luaを使わない場合は、以下のように記述する。
	/*int counter = Time->GetCountedFrames();
	std::vector<std::shared_ptr<Enemy>> enemies;
	if (counter % 15 == 0 && counter <= 70) {
		enemies.push_back(enemyManager->GenerateObject(EnemyManager::EnemyID::SmallBlue, Vector2{ Game::Width / 2.0f, 0.0f }));
		enemies.back()->Spawned(2.0f, M_PI_2, 100);
	}*/
}

bool Script::IsTerminated()
{
	if (tasksList.empty())
		return true;
	else
		return false;
}
