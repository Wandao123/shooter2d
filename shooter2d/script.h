#ifndef SCRIPT_H
#define SCRIPT_H

#include <list>
#include <sol.hpp>
#include <functional>
#include "mover.h"

namespace Shooter {
	// GameScene�N���X�݂̂���Ăяo�����Ƒz��B
	class Script
	{
	public:
		Script(BulletManager& bulletManager, EnemyManager& enemyManager, PlayerManager& playerManager);
		void Run();
	private:
		BulletManager& bulletManager;
		EnemyManager& enemyManager;
		PlayerManager& playerManager;
		sol::state lua;
		// HACK: sol�̃R���[�`���̌Ăяo�����̂��߂ɁA�g�ŕێ�����K�v������BLua����coroutine.create������΁A�X���b�h�����ł��悢�H
		std::list<std::pair<sol::thread, sol::coroutine>> tasksList;  // ����ɓo�^���ꂽ�R���[�`�������t���[�����s�����B

		/// <summary>�G�𐶐�����B</summary>
		/// <param name="id">��������G��ID</param>
		/// <param name="posX">�����ʒu��x���W</param>
		/// <param name="posY">�����ʒu��y���W</param>
		/// <param name="speed">�������x�̑傫���i�����j</param>
		/// <param name="angle">�������x��x������̊p�x</param>
		/// <param name="hitPoint">�G�̗̑�</param>
		/// <returns>�������ꂽ�G�ւ̃|�C���^</returns>
		std::function<std::shared_ptr<Enemy>(const EnemyManager::EnemyID, const float, const float, const float, const float, const int hitPoint)> generateEnemy =
		[this](const EnemyManager::EnemyID id, const float posX, const float posY, const float speed, const float angle, const int hitPoint) -> std::shared_ptr<Enemy> {
			auto newObject = enemyManager.GenerateObject(id, Vector2{ posX, posY });
			newObject->Spawned(speed, angle, hitPoint);
			return std::move(newObject);
		};

		/// <summary>�w�肳�ꂽ�ʒu�ɓG�e�𐶐�����B�������̑��݂͖��Ȃ��B</summary>
		/// <param name="id">��������G�e��ID</param>
		/// <param name="posX">�����ʒu��x���W</param>
		/// <param name="posY">�����ʒu��y���W</param>
		/// <param name="speed">�������x�̑傫���i�����j</param>
		/// <param name="angle">�������x��x������̊p�x</param>
		/// <returns>�������ꂽ�G�e�ւ̃|�C���^</returns>
		std::function<std::shared_ptr<Bullet>(const BulletManager::BulletID, const float, const float, const float, const float)> generateBullet =
		[this](const BulletManager::BulletID id, const float posX, const float posY, const float speed, const float angle) -> std::shared_ptr<Bullet> {
			auto newObject = bulletManager.GenerateObject(id, Vector2{ posX, posY });
			newObject->Shot(speed, angle);
			return std::move(newObject);
		};

		/// <summary>�w�肵���G����G�e�𐶐�����B�����������݂��Ȃ���ΐ������Ȃ��B</summary>
		/// <param name="id">��������G�e��ID</param>
		/// <param name="posX">�����ʒu��x���W</param>
		/// <param name="posY">�����ʒu��y���W</param>
		/// <param name="speed">�������x�̑傫���i�����j</param>
		/// <param name="angle">�������x��x������̊p�x</param>
		/// <returns>�������ꂽ�G�e�ւ̃|�C���^</returns>
		std::function<std::shared_ptr<Bullet>(const BulletManager::BulletID, std::shared_ptr<Enemy>, const float, const float)> generateBulletFromEnemy =
		[this](const BulletManager::BulletID id, std::shared_ptr<Enemy> enemy, const float speed, const float angle) -> std::shared_ptr<Bullet> {
			if (enemy->IsEnabled())
				return std::move(generateBullet(id, enemy->GetPosition().x, enemy->GetPosition().y, speed, angle));
			else
				return nullptr;
		};

		/// <summary>�R���[�`����o�^����B�o�^���ꂽ�R���[�`���͖��t���[�����s�����B</summary>
		/// <param name="func">�o�^����R���[�`��</param>
		std::function<void(const sol::function)> startCoroutine =
		[this](const sol::function func) {
			sol::thread th = sol::thread::create(lua.lua_state());
			sol::coroutine co(th.state(), func);
			co();
			tasksList.push_back(std::make_pair(th, co));
		};

		/// <summary>�R���[�`����o�^����B�o�^���ꂽ�R���[�`���͖��t���[�����s�����B</summary>
		/// <param name="func">�o�^����R���[�`��</param>
		/// <param name="va">�R���[�`���̈������X�g</param>
		std::function<void(const sol::function, sol::variadic_args)> startCoroutineWithArgs =
		[this](const sol::function func, sol::variadic_args va) {
			sol::thread th = sol::thread::create(lua.lua_state());
			sol::coroutine co(th.state(), func);
			co(sol::as_args(std::vector<sol::object>(va.begin(), va.end())));
			tasksList.push_back(std::make_pair(th, co));
		};

		/// <summary>���@�ւ̃|�C���^���擾</summary>
		/// <returns>���@�ւ̃|�C���^</returns>
		std::function<std::shared_ptr<Player>(void)> getPlayer =
		[this]() -> std::shared_ptr<Player> {
			return playerManager.GetPlayer();
		};
	};
}

#endif // !SCRIPT_H