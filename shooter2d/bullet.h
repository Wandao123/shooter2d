#ifndef BULLET_H
#define BULLET_H

#include "mover.h"

namespace Shooter {
	class Bullet : public Mover
	{
	public:
		Bullet(const Vector2<float>& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID, unsigned int damage);
		void Draw() const override;
		void OnCollide(Mover& mover) override;
		virtual void Shot(const float speed, const float angle);  // 実体化関数
	protected:
		Rect<int> clip;  // 具体的な値は継承先で設定せよ。
		Rect<int>& clipFromImage(unsigned int countedFrames) override;
	};

	class BulletManager : public ObjectManager
	{
	public:
		enum class BulletID
		{
			// 敵弾。
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
			RiceBlue,
			// 自弾。
			ReimuNormal,
			MarisaNormal,
			SanaeNormal
		};

		BulletManager();
		std::weak_ptr<Bullet> GenerateObject(const BulletID id, const Vector2<float>& position);
		std::list<std::weak_ptr<Bullet>> GetBullets() const { return getList<Bullet>(); }
	};
}

#endif // !BULLET_H