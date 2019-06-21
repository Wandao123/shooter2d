#include "game.h"
#include "scene.h"

using namespace Shooter;

/// <param name="bulletManager">GameScene�N���X��bulletManager�����o�ϐ�</param>
/// <param name="enemyManager">GameScene�N���X��enemyManager�����o�ϐ�</param>
/// <param name="playerManager">GameScene�N���X��playerManager�����o�ϐ�</param>
Script::Script(BulletManager& bulletManager, EnemyManager& enemyManager, PlayerManager& playerManager)
	: enemyManager(enemyManager)
	, bulletManager(bulletManager)
	, playerManager(playerManager)
{
	// Lua�̏������B
	lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::math, sol::lib::io, sol::lib::string);
	lua.script_file("scripts/stage.lua");  // TODO: �G���[����

	// �e��N���X�̒�`
	lua.new_usertype<Bullet>(
		"Bullet",
		// Lua���Ő�������Ȃ�΁A�R���X�g���N�^���`���� ``Enemy.new(...)'' �Ƃ���B
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

	// �萔�̓o�^�B
	int width = Game::Width, height = Game::Height;  // ���ɑ�������gcc�ŃR���p�C���ł��Ȃ��B
	lua["ScreenWidth"] = width;
	lua["ScreenHeight"] = height;
	lua["EnemyID"] = lua.create_table_with(
		"SmallBlue", EnemyManager::EnemyID::SmallBlue
	);
	lua["BulletID"] = lua.create_table_with(
		"Small", BulletManager::BulletID::Small
	);

	// �֐��Q�̓o�^�B
	lua["GenerateEnemy"] = generateEnemy;
	lua["GenerateBullet"] = sol::overload(
		generateBullet,
		generateBulletFromEnemy
	);
	lua["StartCoroutine"] = sol::overload(
		startCoroutine,
		startCoroutineWithArgs
	);
	lua["GetPlayer"] = getPlayer;  // �萔�Ƃ��Ĉ����ׂ��H�@���̏ꍇ�͎��O�ɐ�������Ă��邱�Ƃ�v������B

	// �X�e�[�W�E�X�N���v�g�̓o�^�B
	startCoroutine(lua["StartStage"]);
}

/// <summary>�v���C���̓G�̏o����e�̐����Ȃǂ̑S�e���L�q����B</summary>
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

	// Lua���g��Ȃ��ꍇ�́A�ȉ��̂悤�ɋL�q����B
	/*int counter = Time->GetCountedFrames();
	std::vector<std::shared_ptr<Enemy>> enemies;
	if (counter % 15 == 0 && counter <= 70) {
		enemies.push_back(enemyManager->GenerateObject(EnemyManager::EnemyID::SmallBlue, Vector2{ Game::Width / 2.0f, 0.0f }));
		enemies.back()->Spawned(2.0f, M_PI_2, 100);
	}*/
}