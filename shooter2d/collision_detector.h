#ifndef COLLISION_DETECTOR_H
#define COLLISION_DETECTOR_H

#include "mover.h"

namespace Shooter {
	// GameSceneクラスのみから呼び出されると想定。
	class CollisionDetector {
	public:
		// スマート・ポインタではなくとも参照で十分なときには参照をつかう。
		CollisionDetector(BulletManager& bulletManager, EffectManager& effectManager, EnemyManager& enemyManager, PlayerManager& playerManager)
			: playerManager(playerManager)
			, enemyManager(enemyManager)
			, bulletManager(bulletManager)
			, effectManager(effectManager)
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