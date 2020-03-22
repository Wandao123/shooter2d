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
	auto relativePosition = mover2.GetPosition() - mover1.GetPosition();
	//auto relativeVelocity = MathUtils::PolarCoordinateToVector<double>(mover2.GetSpeed(), mover2.GetAngle())
	//		- MathUtils::PolarCoordinateToVector<double>(mover1.GetSpeed(), mover1.GetAngle());
	if (mover2.GetCollider().DoesCollideWith(relativePosition, mover1.GetCollider())) {
		mover1.OnCollide(mover2);
		mover2.OnCollide(mover1);
	}
}

void CollisionDetector::Draw() const
{
	// 抽象クラスのままでは半径や大きさを取得できないので、RTTIを使う。現在のGetColliderの返り値は抽象クラスなので、変数に代入できない。代入したければ、最終的なMoverの具象クラスでGetColliderを定義すればよい。
	CircleShape circle;
	RectangleShape rectangle;
	auto drawShape = [&circle, &rectangle](Mover& mover) {
		if (typeid(mover.GetCollider()) == typeid(CircleCollider)) {
			circle.SetRadius(static_cast<int>(dynamic_cast<CircleCollider&>(mover.GetCollider()).GetRadius() * 2.0e0) / 2);
			circle.SetColor(0x00, 0xFF, 0x00, 0xFF);
			circle.Draw(MathUtils::RoundToInt(mover.GetPosition() + mover.GetCollider().GetOffset()));
		} else if (typeid(mover.GetCollider()) == typeid(RectangleCollider)) {
			rectangle.SetSize(MathUtils::RoundToInt(dynamic_cast<RectangleCollider&>(mover.GetCollider()).GetSize()));
			rectangle.SetColor(0x00, 0xFF, 0x00, 0xFF);
			rectangle.Draw(MathUtils::RoundToInt(mover.GetPosition() + mover.GetCollider().GetOffset()) - rectangle.GetSize() / 2);
		}
	};
	for (auto&& enemy : enemyManager.GetEnemies())
		drawShape(*enemy.lock());
	drawShape(*playerManager.GetPlayer().lock());
	for (auto&& bullet : enemyBulletManager.GetBullets())
		drawShape(*bullet.lock());
	for (auto&& bullet : playerBulletManager.GetBullets())
		drawShape(*bullet.lock());
}