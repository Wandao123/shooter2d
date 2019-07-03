#include "enemy.h"

using namespace Shooter;

/******************************** 個別設定用クラス *********************************/

class SmallBlueEnemy : public Enemy
{
public:
	SmallBlueEnemy(const Vector2& position)
		: Enemy(position, std::make_unique<Sprite>("images/Enemy.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, Enemy::Width * 0.5f), EffectManager::EffectID::BlueCircle)
	{
		for (int j = 0; j < static_cast<int>(clips[0].size()); j++) {
			clips[0][j] = { j * Width, 0, Width, Height };                 // 停止時
			clips[1][j] = { (j + 3) * Width, 2 * Height, Width, Height };  // 左移動
			clips[2][j] = { (j + 3) * Width, 0, Width, Height };           // 右移動
		}
	}
};

/******************************** Enemy *********************************/

/// <param name="position">初期位置</param>
/// <param name="sprite">Spriteクラスへのポインタ（画像はAssetLoaderから指定）</param>
/// <param name="collider">当たり判定クラスへのポインタ</param>
/// <param name="effectID">消滅エフェクトのID</param>
Enemy::Enemy(const Vector2& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID)
	: Mover(position, 0.0f, M_PI_2, std::move(sprite), std::move(collider), effectID, 1)
{}

void Enemy::Draw()
{
	if (isDamaged) {
		sprite->SetColor(128, 128, 128);
		isDamaged = false;
	}
	else {
		sprite->SetColor(255, 255, 255);
	}
	Mover::Draw();
}

void Enemy::OnCollide(Mover& mover)
{
	hitPoint -= mover.GetDamage();
	if (hitPoint <= 0)
		enabled = false;
	else
		isDamaged = true;
}

void Enemy::Spawned(const float speed, const float angle, const int hitPoint)
{
	Mover::spawned();
	SetSpeed(speed);
	SetAngle(angle);
	this->hitPoint = hitPoint;
}

SDL_Rect& Enemy::clipFromImage(Uint32 countedFrames)
{
	const int DelayFrames = 6;
	const int NumSlice = 3;
	if (speed > 0.0f) {  // 30度ずつの領域に分けて、画像を切り換える。
		if (angle == M_PI * 3.0f / 2.0f)
			return clips[0][(countedFrames / DelayFrames) % NumSlice];  // どう設定するのが適切なのか？
		else if (angle > M_PI * 11.0f / 12.0f && angle < M_PI * 3.0f / 2.0f)
			return clips[1][2];
		else if (angle > M_PI * 3.0f / 4.0f && angle <= M_PI * 11.0f / 12.0f)
			return clips[1][1];
		else if (angle > M_PI * 7.0f / 12.0f && angle <= M_PI * 3.0f / 4.0f)
			return clips[1][0];
		else if (angle >= M_PI * 5.0f / 12.0f && angle <= M_PI * 7.0f / 12.0f)
			return clips[0][(countedFrames / DelayFrames) % NumSlice];
		else if (angle >= M_PI / 4.0f && angle < M_PI * 5.0f / 12.0f)
			return clips[2][0];
		else if (angle >= M_PI / 12.0f && angle < M_PI / 4.0f)
			return clips[2][1];
		else
			return clips[2][2];
	}
	else {
		return clips[0][(countedFrames / DelayFrames) % NumSlice];
	}
};

/******************************** EnemyManager *********************************/

std::weak_ptr<Enemy> EnemyManager::GenerateObject(const EnemyID id, const Vector2& position)
{
	std::weak_ptr<Enemy> newObject;
	switch (id) {
	case EnemyID::SmallBlue:
		newObject = assignObject<SmallBlueEnemy>(position);
		break;
	}
	return newObject;
}
