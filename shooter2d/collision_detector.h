#ifndef COLLISION_DETECTOR_H
#define COLLISION_DETECTOR_H

#include "mover.h"

namespace Shooter {
	class CollisionDetector {
	public:
		CollisionDetector(std::shared_ptr<PlayerManager> playerManager, std::shared_ptr<EnemyManager> enemyManager, std::shared_ptr<BulletManager> bulletManager, std::shared_ptr<EffectManager> effectManager)
			: playerManager(playerManager)
			, enemyManager(enemyManager)
			, bulletManager(bulletManager)
			, effectManager(effectManager)
		{}

		void CheckAll();
		void CheckBetween(Mover& mover1, Mover& mover2);
	private:
		std::shared_ptr<PlayerManager> playerManager;
		std::shared_ptr<EnemyManager> enemyManager;
		std::shared_ptr<BulletManager> bulletManager;
		std::shared_ptr<EffectManager> effectManager;
	};
}

#endif // !COLLISION_DETECTOR_H