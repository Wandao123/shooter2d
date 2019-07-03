#include "bullet.h"

using namespace Shooter;

/******************************** 個別設定用クラス *********************************/

class LargeRedBullet : public Bullet
{
public:
	LargeRedBullet(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>("images/shot_all.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 21.0f), EffectManager::EffectID::None, 1)
	{
		clip = { 320, 0, 64, 64 };
		sprite->SetBlendModeAdd();
	}
};

class LargeBlueBullet : public Bullet
{
public:
	LargeBlueBullet(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>("images/shot_all.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 21.0f), EffectManager::EffectID::None, 1)
	{
		clip = { 448, 0, 64, 64 };
		sprite->SetBlendModeAdd();
	}
};

class MiddleRedBullet : public Bullet
{
public:
	MiddleRedBullet(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>("images/shot_all.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 9.0f), EffectManager::EffectID::None, 1)
	{
		clip = { 0, 50, 30, 30 };
	}
};

class MiddleBlueBullet : public Bullet
{
public:
	MiddleBlueBullet(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>("images/shot_all.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 9.0f), EffectManager::EffectID::None, 1)
	{
		clip = { 150, 50, 30, 30 };
	}
};

class SmallRedBullet : public Bullet
{
public:
	SmallRedBullet(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>("images/shot_all.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 4.0f), EffectManager::EffectID::None, 1)
	{
		clip = { 1, 13, 16, 16 };
	}
};

class SmallBlueBullet : public Bullet
{
public:
	SmallBlueBullet(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>("images/shot_all.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 4.0f), EffectManager::EffectID::None, 1)
	{
		clip = { 91, 13, 16, 16 };
	}
};

class TinyRedBullet : public Bullet
{
public:
	TinyRedBullet(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>("images/shot_all.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 3.0f), EffectManager::EffectID::None, 1)
	{
		clip = { 1, 2, 9, 9 };
	}
};

class TinyBlueBullet : public Bullet
{
public:
	TinyBlueBullet(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>("images/shot_all.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 3.0f), EffectManager::EffectID::None, 1)
	{
		clip = { 61, 2, 9, 9 };
	}
};

class ScaleRedBullet : public Bullet
{
public:
	ScaleRedBullet(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>("images/shot_all.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 3.0f), EffectManager::EffectID::None, 1)
	{
		clip = { 1, 209, 15, 15 };
	}
};

class ScaleBlueBullet : public Bullet
{
public:
	ScaleBlueBullet(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>("images/shot_all.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 3.0f), EffectManager::EffectID::None, 1)
	{
		clip = { 91, 209, 15, 15 };
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

void Bullet::Draw() const
{
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

SDL_Rect& Bullet::clipFromImage(unsigned int countedFrames)
{
	return clip;
}

/******************************** BulletManager *********************************/

std::weak_ptr<Bullet> BulletManager::GenerateObject(const BulletID id, const Vector2& position)
{
	std::weak_ptr<Bullet> newObject;
	switch (id) {
	case BulletID::LargeRed:
		newObject = assignObject<LargeRedBullet>(position);
		break;
	case BulletID::LargeBlue:
		newObject = assignObject<LargeBlueBullet>(position);
		break;
	case BulletID::MiddleRed:
		newObject = assignObject<MiddleRedBullet>(position);
		break;
	case BulletID::MiddleBlue:
		newObject = assignObject<MiddleBlueBullet>(position);
		break;
	case BulletID::SmallRed:
		newObject = assignObject<SmallRedBullet>(position);
		break;
	case BulletID::SmallBlue:
		newObject = assignObject<SmallBlueBullet>(position);
		break;
	case BulletID::TinyRed:
		newObject = assignObject<TinyRedBullet>(position);
		break;
	case BulletID::TinyBlue:
		newObject = assignObject<TinyBlueBullet>(position);
		break;
	case BulletID::ScaleRed:
		newObject = assignObject<ScaleRedBullet>(position);
		break;
	case BulletID::ScaleBlue:
		newObject = assignObject<ScaleBlueBullet>(position);
		break;
	}
	return newObject;
}
