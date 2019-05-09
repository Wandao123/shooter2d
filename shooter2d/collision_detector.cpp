#include "collision_detector.h"

using namespace Shooter;

void CollisionDetector::CheckAll()
{
	// HACK: 判定する対象を減らす。四分木法など。
	for (auto&& player : playerManager->GetList())
		for (auto&& enemy : enemyManager->GetList())
			CheckBetween(*(std::dynamic_pointer_cast<Mover>(player)), *(std::dynamic_pointer_cast<Mover>(enemy)));
}

void CollisionDetector::CheckBetween(Mover& mover1, Mover& mover2)
{
	// HACK: すり抜け防止。
	if (mover1.GetCollider().DoesCollideWith(mover2.GetCollider())) {
		mover1.OnTriggerEnter();
		mover2.OnTriggerEnter();
	}
}