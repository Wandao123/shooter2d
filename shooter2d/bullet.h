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
		virtual void Shot(const float speed, const float angle);  // 実体化関数
	protected:
		SDL_Rect clip;  // 具体的な値は継承先で設定せよ。
		SDL_Rect& clipFromImage(unsigned int countedFrames) override;
	};

	class BulletManager : public ObjectManager
	{
	public:
		// HACK: 同じようなコードを二通り書く必要があるため、敵弾と自弾とに分けるのは非効率的か？
		enum class BulletID  // 敵弾。
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
			ScaleBlue,
			RiceRed,
			RiceBlue
		};
		enum class ShotID  // 自弾。
		{
			ReimuNormal,
			MarisaNormal,
			SanaeNormal
		};

		BulletManager();
		std::weak_ptr<Bullet> GenerateObject(const BulletID id, const Vector2& position);
		std::weak_ptr<Bullet> GenerateObject(const ShotID id, const Vector2& position);
		std::list<std::weak_ptr<Bullet>> GetBullets() const { return getList<Bullet>(); }
	};
}

#endif // !BULLET_H
