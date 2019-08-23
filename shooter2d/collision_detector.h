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
		CollisionDetector(BulletManager& bulletManager, EffectManager& effectManager, EnemyManager& enemyManager, PlayerManager& playerManager)
			: bulletManager(bulletManager)
			, effectManager(effectManager)
			, enemyManager(enemyManager)
			, playerManager(playerManager)
		{}

		void CheckAll();
		void CheckBetween(Mover& mover1, Mover& mover2);
	private:
		BulletManager& bulletManager;
		EffectManager& effectManager;
		EnemyManager& enemyManager;
		PlayerManager& playerManager;
	};
}

#endif // !COLLISION_DETECTOR_H