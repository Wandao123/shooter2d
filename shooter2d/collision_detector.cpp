#include "collision_detector.h"

using namespace Shooter;

void CollisionDetector::CheckAll()
{
	// TODO: 判定する対象を減らす。四分木法など。
	auto player = playerManager.GetPlayer();
	if (!player.expired()) {
		for (auto&& bullet : enemyBulletManager.GetBullets())
			CheckBetween(*player.lock(), *bullet.lock());
		for (auto&& enemy : enemyManager.GetEnemies())
			CheckBetween(*player.lock(), *enemy.lock());
	}
	for (auto&& bullet : playerBulletManager.GetBullets())
		for (auto&& enemy : enemyManager.GetEnemies())
			CheckBetween(*bullet.lock(), *enemy.lock());
}

void CollisionDetector::CheckBetween(Mover& mover1, Mover& mover2)
{
	if (!mover1.IsEnabled() || !mover2.IsEnabled())
		return;
	Vector2<double> relativeVector = mover2.GetPosition() - mover1.GetPosition();
	// TODO: すり抜け防止。
	if (mover2.GetCollider().DoesCollideWith(relativeVector, mover1.GetCollider())) {
		mover1.OnCollide(mover2);
		mover2.OnCollide(mover1);
	}
	if (mover1.GetHitPoint() <= 0)
		effectManager.GenerateObject(mover1.GetEffectID(), mover1.GetPosition()).lock()->Played();
	if (mover2.GetHitPoint() <= 0)
		effectManager.GenerateObject(mover2.GetEffectID(), mover2.GetPosition()).lock()->Played();
}
