#include "scene.h"  // ここで script.h は読み込み済み。
#include "media.h"
#include <algorithm>

using namespace Shooter;

/// <param name="enemyManager">GameSceneクラスのenemyManagerメンバ変数</param>
/// <param name="enemyBulletManager">GameSceneクラスのenemyBulletManagerメンバ変数</param>
/// <param name="playerManager">GameSceneクラスのplayerManagerメンバ変数</param>
/// <param name="playerBulletManager">GameSceneクラスのplayerBulletManagerメンバ変数</param>
Script::Script(EffectManager& effectManager, EnemyManager& enemyManager, BulletManager& enemyBulletManager, PlayerManager& playerManager, BulletManager& playerBulletManager)
	: effectManager(effectManager)
	, enemyManager(enemyManager)
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
		// Lua側で生成するならば、コンストラクタを定義して ``Bullet.new(...)'' とする。
		"Angle", sol::property(&Bullet::GetAngle, &Bullet::SetAngle),
		"Erase", &Bullet::Erase,
		"IsEnabled", &Bullet::IsEnabled,
		"PosX", sol::property(
			[](Bullet& bullet) -> double { return bullet.GetPosition().x; },
			[](Bullet& bullet, const double posX) { bullet.SetPosition({ posX, bullet.GetPosition().y }); }),
		"PosY", sol::property(
			[](Bullet& bullet) -> double { return bullet.GetPosition().y; },
			[](Bullet& bullet, const double posY) { bullet.SetPosition({ bullet.GetPosition().x, posY }); }),
		"Speed", sol::property(&Bullet::GetSpeed, &Bullet::SetSpeed),
		"TurnInvincible", &Bullet::TurnInvincible
	);
	lua.new_usertype<Effect>(
		"Effect",
		"Erase", &Effect::Erase,
		"IsEnabled", &Effect::IsEnabled,
		"PosX", sol::property(
			[](Effect& effect) -> double { return effect.GetPosition().x; },
			[](Effect& effect, const double posX) { effect.SetPosition({ posX, effect.GetPosition().y }); }),
		"PosY", sol::property(
			[](Effect& effect) -> double { return effect.GetPosition().y; },
			[](Effect& effect, const double posY) { effect.SetPosition({ effect.GetPosition().x, posY }); })
	);
	lua.new_usertype<Enemy>(
		"Enemy",
		"Angle", sol::property(&Enemy::GetAngle, &Enemy::SetAngle),
		"Erase", &Enemy::Erase,
		"HitPoint", sol::property([](Enemy& enemy) -> int { return enemy.GetHitPoint(); }),
		"IsEnabled", &Enemy::IsEnabled,
		"PosX", sol::property(
			[](Enemy& enemy) -> double { return enemy.GetPosition().x; },
			[](Enemy& enemy, const double posX) { enemy.SetPosition({ posX, enemy.GetPosition().y }); }),
		"PosY", sol::property(
			[](Enemy& enemy) -> double { return enemy.GetPosition().y; },
			[](Enemy& enemy, const double posY) { enemy.SetPosition({ enemy.GetPosition().x, posY }); }),
		"Speed", sol::property(&Enemy::GetSpeed, &Enemy::SetSpeed),
		"TurnInvincible", &Enemy::TurnInvincible
	);
	lua.new_usertype<Player>(
		"Player",
		"Angle", sol::property(&Player::GetAngle),
		"IsEnabled", &Player::IsEnabled,
		"Life", sol::property([](Player& player) -> int { return player.GetLife(); }),
		"PosX", sol::property(
			[](Player& player) -> double { return player.GetPosition().x; },
			[](Player& player, const double posX) { player.SetPosition({ posX, player.GetPosition().y }); }),
		"PosY", sol::property(
			[](Player& player) -> double { return player.GetPosition().y; },
			[](Player& player, const double posY) { player.SetPosition({ player.GetPosition().x, posY }); }),
		"SetVelocity", [](Player& player, const double dirX, const double dirY, const bool slowMode) { player.SetVelocity({ dirX, dirY }, slowMode); },
		"Spawned", &Player::Spawned,
		"Speed", sol::property(&Player::GetSpeed),
		"TurnInvincible", &Player::TurnInvincible
	);
	lua.new_usertype<PlayerOption>(
		"PlayerOption",
		"Angle", sol::property(&PlayerOption::GetAngle),
		"IsEnabled", &PlayerOption::IsEnabled,
		"PosX", sol::property(
			[](PlayerOption& option) -> double { return option.GetPosition().x; },
			[](PlayerOption& option, const double posX) { option.SetPosition({ posX, option.GetPosition().y }); }),
		"PosY", sol::property(
			[](PlayerOption& option) -> double { return option.GetPosition().y; },
			[](PlayerOption& option, const double posY) { option.SetPosition({ option.GetPosition().x, posY }); }),
		"Spawned", &PlayerOption::Spawned,
		"Speed", sol::property(&PlayerOption::GetSpeed)
	);

	// 定数の登録。
	lua["ScreenWidth"] = Media::Create().GetWidth();
	lua["ScreenHeight"] = Media::Create().GetHeight();
	int width = Player::Width, height = Player::Height;  // 直に代入するとgccでコンパイルできない。
	lua["PlayerWidth"] = width;
	lua["PlayerHeight"] = height;
	lua.new_enum(
		"BulletID",
		// 敵弾。
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
		"RiceBlue", BulletManager::BulletID::RiceBlue,
		// 自弾。
		"ReimuNormal", BulletManager::BulletID::ReimuNormal,
		"MarisaNormal", BulletManager::BulletID::MarisaNormal,
		"SanaeNormal", BulletManager::BulletID::SanaeNormal
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
	lua.new_enum(
		"EffectID",
		"None", EffectManager::EffectID::None,
		"DefetedPlayer", EffectManager::EffectID::DefetedPlayer,
		"RedCircle", EffectManager::EffectID::RedCircle,
		"BlueCircle", EffectManager::EffectID::BlueCircle,
		"EnemyShotSound", EffectManager::EffectID::EnemyShotSound,
		"PlayerShotSound", EffectManager::EffectID::PlayerShotSound
	);
	lua.new_enum(
		"EnemyID",
		"SmallRed", EnemyManager::EnemyID::SmallRed,
		"SmallBlue", EnemyManager::EnemyID::SmallBlue
	);
	lua.new_enum(
		"PlayerID",
		"Reimu", PlayerManager::PlayerID::Reimu,
		"Marisa", PlayerManager::PlayerID::Marisa,
		"Sanae", PlayerManager::PlayerID::Sanae/*,
		"ReimuOption", PlayerManager::PlayerID::ReimuOption,
		"MarisaOption", PlayerManager::PlayerID::MarisaOption,
		"SanaeOption", PlayerManager::PlayerID::SanaeOption*/
	);
	lua.new_enum(
		"SceneID",
		"Title", SceneID::Title,
		"StageClear", SceneID::StageClear,
		"AllClear", SceneID::AllClear
	);

	// 関数群の登録。
	lua["GenerateBullet"] = sol::overload(
		generateEnemyBullet,
		generateBulletFromEnemy
	);
	lua["GenerateEffect"] = sol::overload(
		generateEffect,
		// 非視覚エフェクト用に位置は省略可とする。solではデフォルト引数を指定できないので、ラムダ式とオーバーロードを用いる。
		[this](const EffectManager::EffectID id) -> std::shared_ptr<Effect> { return generateEffect(id, 0.e0, 0.e0); }
	);
	lua["GenerateEnemy"] = generateEnemy;
	lua["GeneratePlayer"] = generatePlayer;
	lua["GeneratePlayerBullet"] = generatePlayerBullet;
	lua["ChangeScene"] = changeScene;
	lua["StartCoroutine"] = sol::overload(
		startCoroutine,
		startCoroutineWithArgs
	);
	lua["StopCoroutine"] = stopCoroutine;
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
	if (tasksList.empty())
		status = Status::Dead;  // 自機スクリプトの実装方法との関係上、基本的にここは実行されない。
	else
		status = Status::Running;
	tasksList.remove_if([](Task task) {  // 終了したスレッドを全て削除。削除しなければ再実行されることに注意。また、無限ループがある場合も削除されない。
		if (task.first.status() == sol::thread_status::dead) {
			task.first.state().collect_garbage();  // タイミングがよく判らないため、明示的に実行。もしスレッドが終了していれば、ObjectManager::objectsListの各要素の参照カウントが1になる筈。
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
		enemies.push_back(enemyManager->GenerateObject(EnemyManager::EnemyID::SmallBlue, Vector2<double>{ Media::Create().GetWidth() / 2.0, 0.0 }));
		enemies.back()->Spawned(2.0, M_PI_2, 100);
	}
	return status*/
}

/// <summary>スクリプトのファイルを読み込む。</summary>
/// <param name="filename">スクリプトのファイルへのパス</param>
void Script::LoadFile(const std::string filename)
{
	lua.script_file(filename);  // TODO: エラー処理、二重読み込み？
}

/// <summary>コルーチンを登録する。</summary>
/// <param name="name">登録する関数</param>
/// <remarks>startCoroutineと殆ど同じ。</remarks>
void Script::RegisterFunction(const std::string name)
{
	sol::thread th = sol::thread::create(lua.lua_state());
	sol::coroutine co = th.state()[name];
	co();
	tasksList.push_back(std::make_pair(th, co));
}
