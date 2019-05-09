#ifndef COLLISION_DETECTOR_H
#define COLLISION_DETECTOR_H

#include "mover.h"

namespace Shooter {
	class CollisionDetector {
	public:
		CollisionDetector(std::shared_ptr<PlayerManager> playerManager, std::shared_ptr<EnemyManager> enemyManager)
			: playerManager(playerManager)
			, enemyManager(enemyManager)
		{}

		void CheckAll();
		void CheckBetween(Mover& mover1, Mover& mover2);
	private:
		std::shared_ptr<PlayerManager> playerManager;
		std::shared_ptr<EnemyManager> enemyManager;
	};
}

#endif // !COLLISION_DETECTOR_H