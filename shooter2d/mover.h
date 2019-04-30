#ifndef MOVER_H
#define MOVER_H

#include <array>
#include "game_object.h"
#include "sprite.h"

namespace Shooter {
	class Mover : public GameObject
	{
	public:
		Mover(const Vector2 position, const float speed,
				const float angle, const std::shared_ptr<Sprite> sprite)
			: GameObject(position)
			, speed(speed)
			, angle(angle)
			, sprite(sprite) {}
		virtual ~Mover() {}
		virtual void Draw() override;
		virtual void Update() override;

		void SetSprite(const std::shared_ptr<Sprite> sprite)
		{
			this->sprite = sprite;
		}

		float GetSpeed() const
		{
			return speed;
		}

		void SetSpeed(float speed)
		{
			this->speed = speed;
		}

		float GetAngle() const
		{
			return angle;
		}

		void SetAngle(float angle)
		{
			this->angle = std::fmod(angle, 2 * M_PI);
		}
	protected:
		float speed;  // 単位：ドット毎フレーム
		float angle;  // x軸から時計回りに回転したときの角度。
		std::shared_ptr<Sprite> sprite;
		bool isVisible1Sec() const;
	};

	// TODO: 後々、抽象クラスにして機体の種類毎にクラスを分ける。
	class Player : public Mover
	{
	public:
		static const int Height = 48;
		static const int Width = 32;
		Player(const Vector2 position, const float highSpeed, const float lowSpeed);
		~Player() = default;
		void Draw() override;
		void Update() override;
	private:
		std::array<std::array<SDL_Rect, 5>, 3> clips;
		float lowSpeed;
		Vector2 velocity;  // 自機はxy座標系の方が扱いやすい。
		void move();
	};

	class Enemy : public Mover
	{
	public:
		static const int Height = 32;
		static const int Width = 32;
		Enemy(const Vector2 position, const float speed, const float angle);
		~Enemy() = default;
		void Draw() override;
		void Update() override;
	private:
		std::array<std::array<SDL_Rect, 3>, 3> clips;
	};

	enum class EnemyID
	{
		SmallBlue
	};

	class EnemyManager : public ObjectManager
	{
	public:
		std::shared_ptr<Enemy> GenerateObject(const EnemyID id, const float posX, const float posY, const float speed, const float angle);
	};
}

#endif // !MOVER_H
