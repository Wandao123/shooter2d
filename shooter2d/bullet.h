#ifndef BULLET_H
#define BULLET_H

#include "mover.h"

namespace Shooter {
	class Bullet : public Mover
	{
	public:
		Bullet(const Vector2& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID, unsigned int damage);
		void Draw() const override;
		void OnCollide(Mover& mover) override;
		void Shot(const float speed, const float angle);  // 実体化関数
	protected:
		SDL_Rect clip;  // 具体的な値は継承先で設定せよ。
		SDL_Rect& clipFromImage(unsigned int countedFrames) override;
	};

	class BulletManager : public ObjectManager
	{
	public:
		enum class BulletID
		{
			LargeRed,
			LargeBlue,
			MiddleRed,
			MiddleBlue,
			SmallRed,
			SmallBlue,
			TinyRed,
			TinyBlue,
			ScaleRed,
			ScaleBlue
		};

		std::weak_ptr<Bullet> GenerateObject(const BulletID id, const Vector2& position);
	};
}

#endif // !BULLET_H
