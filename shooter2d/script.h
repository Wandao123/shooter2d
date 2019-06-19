#ifndef SCRIPT_H
#define SCRIPT_H

#include <sol.hpp>
#include <functional>
#include "mover.h"

namespace Shooter {
	class GameScene;

	class Script
	{
	public:
		Script(const GameScene& gameScene);
		~Script();
		void Run();
	private:
		const GameScene& gameScene;
		sol::state lua;
		// HACK: sol�̃R���[�`���̌Ăяo�����̂��߂ɁA�g�ŕێ�����K�v������BLua����coroutine.create������΁A�X���b�h�����ł��悢�H
		std::list<std::pair<sol::thread, sol::coroutine>> tasksList;  // ����ɓo�^���ꂽ�R���[�`�������t���[�����s�����B

		// �G�̐����B��ނ��������Ƃ��ɖʓ|�Ȃ̂ŁALua���ł͂Ȃ�C++���Ő�������B
		std::shared_ptr<Enemy> generateEnemy(const EnemyManager::EnemyID id, const float posX, const float posY, const float speed, const float angle, const int hitPoint);

		// �w�肳�ꂽ�ʒu�ɓG�e�𐶐�����B�������̑��݂͖��Ȃ��B
		std::shared_ptr<Bullet> generateBullet(const BulletManager::BulletID id, const float posX, const float posY, const float speed, const float angle);

		// �w�肵���G����G�e�𐶐�����B�����������݂��Ȃ���ΐ������Ȃ��B
		std::shared_ptr<Bullet> generateBulletFromEnemy(const BulletManager::BulletID id, std::shared_ptr<Enemy> enemy, const float speed, const float angle);

		// �{���̓R���[�`���𒼐ڎ󂯎���Ď��s�������B�������A�X���b�h�𐶐�������ŌĂяo���ɂ́A������Ŋ֐������󂯎��K�v������B
		void startCoroutine(const std::string name);

		void startCoroutineWithArgs(const std::string name, sol::variadic_args va);
	};
}

#endif // !SCRIPT_H