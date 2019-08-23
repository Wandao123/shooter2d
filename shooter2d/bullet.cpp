#include "bullet.h"

using namespace Shooter;

/******************************** 個別設定用クラス *********************************/

class EnemyBullet : public Bullet
{
public:
	EnemyBullet(const Vector2& position, std::unique_ptr<Collider>&& collider)
		: Bullet(position, std::make_unique<Sprite>("images/shot_all.png", Sprite::BlendMode::None), std::move(collider), EffectManager::EffectID::None, 1)
		, sound(std::make_unique<Sound>("se/shot1.wav"))
	{
		sound->SetVolume(Sound::MaxVolume / 4);
	}

	EnemyBullet(const Vector2& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider)
		: Bullet(position, std::move(sprite), std::move(collider), EffectManager::EffectID::None, 1)
		, sound(std::make_unique<Sound>("se/shot1.wav"))
	{
		sound->SetVolume(Sound::MaxVolume / 4);
	}

	void Shot(const float speed, const float angle) override
	{
		Bullet::Shot(speed, angle);
		sound->Played();
	}
private:
	std::unique_ptr<Sound> sound;
};

class LargeRedBullet : public EnemyBullet
{
public:
	LargeRedBullet(const Vector2& position)
		: EnemyBullet(position, std::make_unique<Sprite>("images/shot_all.png", Sprite::BlendMode::Add), std::make_unique<CircleCollider>(21.0f))
	{
		clip = { 320, 0, 64, 64 };
	}
};

class LargeBlueBullet : public EnemyBullet
{
public:
	LargeBlueBullet(const Vector2& position)
		: EnemyBullet(position, std::make_unique<Sprite>("images/shot_all.png", Sprite::BlendMode::Add), std::make_unique<CircleCollider>(21.0f))
	{
		clip = { 448, 0, 64, 64 };
	}
};

class MiddleRedBullet : public EnemyBullet
{
public:
	MiddleRedBullet(const Vector2& position)
		: EnemyBullet(position, std::make_unique<CircleCollider>(9.0f))
	{
		clip = { 0, 50, 30, 30 };
	}
};

class MiddleBlueBullet : public EnemyBullet
{
public:
	MiddleBlueBullet(const Vector2& position)
		: EnemyBullet(position, std::make_unique<CircleCollider>(9.0f))
	{
		clip = { 150, 50, 30, 30 };
	}
};

class SmallRedBullet : public EnemyBullet
{
public:
	SmallRedBullet(const Vector2& position)
		: EnemyBullet(position, std::make_unique<CircleCollider>(4.0f))
	{
		clip = { 1, 13, 16, 16 };
	}
};

class SmallBlueBullet : public EnemyBullet
{
public:
	SmallBlueBullet(const Vector2& position)
		: EnemyBullet(position, std::make_unique<CircleCollider>(4.0f))
	{
		clip = { 91, 13, 16, 16 };
	}
};

class TinyRedBullet : public EnemyBullet
{
public:
	TinyRedBullet(const Vector2& position)
		: EnemyBullet(position, std::make_unique<CircleCollider>(3.0f))
	{
		clip = { 1, 2, 9, 9 };
	}
};

class TinyBlueBullet : public EnemyBullet
{
public:
	TinyBlueBullet(const Vector2& position)
		: EnemyBullet(position, std::make_unique<CircleCollider>(3.0f))
	{
		clip = { 61, 2, 9, 9 };
	}
};

class ScaleRedBullet : public EnemyBullet
{
public:
	ScaleRedBullet(const Vector2& position)
		: EnemyBullet(position, std::make_unique<CircleCollider>(3.0f))
	{
		clip = { 1, 209, 15, 15 };
	}
};

class ScaleBlueBullet : public EnemyBullet
{
public:
	ScaleBlueBullet(const Vector2& position)
		: EnemyBullet(position, std::make_unique<CircleCollider>(3.0f))
	{
		clip = { 91, 209, 15, 15 };
	}
};

class RiceRedBullet : public EnemyBullet
{
public:
	RiceRedBullet(const Vector2& position)
		: EnemyBullet(position, std::make_unique<CircleCollider>(3.0f))
	{
		clip = { 1, 101, 9, 16 };
	}
};

class RiceBlueBullet : public EnemyBullet
{
public:
	RiceBlueBullet(const Vector2& position)
		: EnemyBullet(position, std::make_unique<CircleCollider>(3.0f))
	{
		clip = { 61, 101, 9, 16 };
	}
};

/******************************** Bullet *********************************/

/// <param name="position">初期位置</param>
/// <param name="sprite">Spriteクラスへのポインタ（画像はAssetLoaderから指定）</param>
/// <param name="collider">当たり判定クラスへのポインタ</param>
/// <param name="effectID">消滅エフェクトのID</param>
/// <param name="damage">衝突時に相手に与えるダメージ</param>
Bullet::Bullet(const Vector2& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID, unsigned int damage)
	: Mover(position, 0.0f, M_PI_2, std::move(sprite), std::move(collider), effectID, damage, 0)
{}

void Bullet::Draw() const
{
	sprite->Draw(position, angle + M_PI_2, 1.0f);  // 元の画像は -PI/2 の向きが正位置。よって、画像の回転角は +PI/2 される。
}

void Bullet::OnCollide(Mover& mover)
{
	enabled = false;
	hitPoint = 0;
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

BulletManager::BulletManager()
{
	std::list<std::shared_ptr<Bullet>> bullets;
	// HACK: enum class の定数を全て指定する、もっと簡単な方法？　テンプレートなどを駆使すれば可能だが……。
	// 注意：IDの最初と最後に依存する。
	for (int i = static_cast<int>(BulletID::LargeRed); i < static_cast<int>(BulletID::RiceBlue); i++) {
		for (int j = 0; j < 50; j++) {
			bullets.push_back(GenerateObject(static_cast<BulletID>(i), Vector2{ 0.0f, 0.0f }).lock());  // 予め生成する。
		}
	}
}

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
	case BulletID::RiceRed:
		newObject = assignObject<RiceRedBullet>(position);
		break;
	case BulletID::RiceBlue:
		newObject = assignObject<RiceBlueBullet>(position);
		break;
	}
	return newObject;
}
