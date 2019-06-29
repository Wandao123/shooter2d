#include "bullet.h"

using namespace Shooter;

/******************************** 個別設定用クラス *********************************/

class SmallBullet : public Bullet
{
public:
	SmallBullet(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>("images/shot_all.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 7.0f), EffectManager::EffectID::BlueCircle, 1)
	{
		clip = { 1, 13, 15, 15 };
	}
};

/******************************** Bullet *********************************/

/// <param name="position">初期位置</param>
/// <param name="sprite">Spriteクラスへのポインタ（画像はAssetLoaderから指定）</param>
/// <param name="collider">当たり判定クラスへのポインタ</param>
/// <param name="effectID">消滅エフェクトのID</param>
/// <param name="damage">衝突時に相手に与えるダメージ</param>
Bullet::Bullet(const Vector2& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID, unsigned int damage)
	: Mover(position, 0.0f, M_PI_2, std::move(sprite), std::move(collider), effectID, damage)
{}

void Bullet::Draw()
{
	SDL_Rect& currentClip = clipFromImage(Time->GetCountedFrames());
	sprite->SetClip(currentClip);
	sprite->Draw(position, angle + M_PI_2, 1.0f);  // 元の画像は -PI/2 の向きが正位置。よって、画像の回転角は +PI/2 される。
}

void Bullet::OnCollide(Mover& mover)
{
	enabled = false;
}

void Bullet::Shot(const float speed, const float angle)
{
	Mover::spawned();
	SetSpeed(speed);
	SetAngle(angle);
}

SDL_Rect& Bullet::clipFromImage(Uint32 countedFrames)
{
	return clip;
}

/******************************** BulletManager *********************************/

std::weak_ptr<Bullet> BulletManager::GenerateObject(const BulletID id, const Vector2& position)
{
	std::weak_ptr<Bullet> newObject;
	switch (id) {
	case BulletID::Small:
		newObject = assignObject<SmallBullet>(position);
		break;
	}
	return newObject;
}