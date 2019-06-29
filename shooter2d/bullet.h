﻿#ifndef BULLET_H
#define BULLET_H

#include "mover.h"

namespace Shooter {
	class Bullet : public Mover
	{
	public:
		Bullet(const Vector2& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID, unsigned int damage);
		void Draw() override;
		void OnCollide(Mover& mover) override;
		void Shot(const float speed, const float angle);  // 実体化関数
	protected:
		SDL_Rect clip;  // 具体的な値は継承先で設定せよ。
		SDL_Rect& clipFromImage(Uint32 countedFrames) override;
	};

	class BulletManager : public ObjectManager
	{
	public:
		enum class BulletID
		{
			Small
		};

		std::weak_ptr<Bullet> GenerateObject(const BulletID id, const Vector2& position);
	};
}

#endif // !BULLET_H