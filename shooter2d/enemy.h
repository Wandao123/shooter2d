#ifndef ENEMY_H
#define ENEMY_H

#include "mover.h"

namespace Shooter {
	class Enemy : public Mover
	{
	public:
		static const int Height = 32;
		static const int Width = 32;
		Enemy(const Vector2& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID);
		~Enemy() = default;
		void Update() override;
		void OnCollide(Mover& mover) override;
		void Spawned(const float speed, const float angle, const int hitPoint);  // 実体化関数
	protected:
		std::array<std::array<SDL_Rect, 3>, 3> clips;  // 具体的な値は継承先で設定せよ。
		bool isDamaged = false;  // 被弾したか否か。
		std::unique_ptr<Sound> sound;
		SDL_Rect& clipFromImage(unsigned int countedFrames) override;
	};

	class EnemyManager : public ObjectManager
	{
	public:
		enum class EnemyID
		{
			SmallRed,
			SmallBlue
		};

		std::weak_ptr<Enemy> GenerateObject(const EnemyID id, const Vector2& position);
	};
}

#endif // !ENEMY_H
