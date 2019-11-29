#ifndef COLLISION_DETECTOR_H
#define COLLISION_DETECTOR_H

#include "bullet.h"
#include "enemy.h"
#include "player.h"

namespace Shooter {
	// GameSceneクラスのみから呼び出されると想定。
	class CollisionDetector {
	public:
		// スマート・ポインタではなくとも参照で十分なときには参照をつかう。
		CollisionDetector(EffectManager& effectManager, EnemyManager& enemyManager, BulletManager& enemyBulletManager, PlayerManager& playerManager, BulletManager& playerBulletManager)
			: effectManager(effectManager)
			, enemyManager(enemyManager)
			, enemyBulletManager(enemyBulletManager)
			, playerManager(playerManager)
			, playerBulletManager(playerBulletManager)
		{}

		void CheckAll();
		void CheckBetween(Mover& mover1, Mover& mover2);
	private:
		EffectManager& effectManager;
		EnemyManager& enemyManager;
		BulletManager& enemyBulletManager;
		PlayerManager& playerManager;
		BulletManager& playerBulletManager;
	};
}

#endif // !COLLISION_DETECTOR_H
