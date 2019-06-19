#include "game.h"
#include "scene.h"

using namespace Shooter;

Script::Script(const GameScene& gameScene)
	: gameScene(gameScene)
{
	// Lua�̏������B
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::math, sol::lib::io, sol::lib::string);
	lua.script_file("scripts/stage.lua");  // TODO: �G���[����

	// Lua�Ŏg���萔�̓o�^�B
	int width = Game::Width, height = Game::Height;  // ���ɑ�������gcc�ŃR���p�C���ł��Ȃ��B
	lua["ScreenWidth"] = width;
	lua["ScreenHeight"] = height;
	lua["EnemyID"] = lua.create_table_with(
		"SmallBlue", EnemyManager::EnemyID::SmallBlue
	);
	lua["BulletID"] = lua.create_table_with(
		"Small", BulletManager::BulletID::Small
	);

	// Lua�Ŏg���֐��Q�̓o�^�B
	lua.new_usertype<Enemy>(
		"Enemy",
		// Lua���Ő�������Ȃ�΁A�R���X�g���N�^���`���� ``Enemy.new(...)'' �Ƃ���B
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
	namespace ph = std::placeholders;
	lua["GenerateEnemy"] = std::bind(&Script::generateEnemy, this, ph::_1, ph::_2, ph::_3, ph::_4, ph::_5, ph::_6);
	lua["GenerateBullet"] = sol::overload(
		std::bind(&Script::generateBullet, this, ph::_1, ph::_2, ph::_3, ph::_4, ph::_5),
		std::bind(&Script::generateBulletFromEnemy, this, ph::_1, ph::_2, ph::_3, ph::_4)
	);
	lua["StartCoroutine"] = sol::overload(
		std::bind(&Script::startCoroutine, this, ph::_1),
		std::bind(&Script::startCoroutineWithArgs, this, ph::_1)
	);
	lua["GetPlayerPosX"] = [&gameScene]() -> float { return gameScene.playerManager->GetPlayer()->GetPosition().x; };
	lua["GetPlayerPosY"] = [&gameScene]() -> float { return gameScene.playerManager->GetPlayer()->GetPosition().y; };

	// �X�e�[�W�E�X�N���v�g�̓o�^�B
	startCoroutine("StartStage");
}

Script::~Script()
{
}

void Script::Run()
{
	tasksList.remove_if([](std::pair<sol::thread, sol::coroutine> task) {  // �I�������X���b�h��S�č폜�B
		if (task.first.status() == sol::thread_status::dead) {
			task.first.state().collect_garbage();  // �^�C�~���O���悭����Ȃ����߁A�����I�Ɏ��s�B�����X���b�h���I�����Ă���΁AObjectManager::objectsList�̊e�v�f�̎Q�ƃJ�E���g��1�ɂȂ锤�B
			return true;
		}
		else {
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
std::shared_ptr<Enemy> Script::generateEnemy(const EnemyManager::EnemyID id, const float posX, const float posY, const float speed, const float angle, const int hitPoint)
{
	auto newObject = gameScene.enemyManager->GenerateObject(id, Vector2{ posX, posY });
	newObject->Spawned(speed, angle, hitPoint);
	return std::move(newObject);
}

std::shared_ptr<Bullet> Script::generateBullet(const BulletManager::BulletID id, const float posX, const float posY, const float speed, const float angle)
{
	auto newObject = gameScene.bulletManager->GenerateObject(id, Vector2{ posX, posY });
	newObject->Shot(speed, angle);
	return std::move(newObject);
}

std::shared_ptr<Bullet> Script::generateBulletFromEnemy(const BulletManager::BulletID id, std::shared_ptr<Enemy> enemy, const float speed, const float angle)
{
	if (enemy->IsEnabled())
		return std::move(generateBullet(id, enemy->GetPosition().x, enemy->GetPosition().y, speed, angle));
	else
		return nullptr;
}

void Script::startCoroutine(const std::string name)
{
	sol::thread th = sol::thread::create(lua.lua_state());
	sol::coroutine co = th.state()[name];
	co();
	tasksList.push_back(std::make_pair(th, co));
}

void Script::startCoroutineWithArgs(const std::string name, sol::variadic_args va)
{
	sol::thread th = sol::thread::create(lua.lua_state());
	sol::coroutine co = th.state()[name];
	co(sol::as_args(std::vector<sol::object>(va.begin(), va.end())));
	tasksList.push_back(std::make_pair(th, co));
}