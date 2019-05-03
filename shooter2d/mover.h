#ifndef MOVER_H
#define MOVER_H

#include <array>
#include "game_object.h"
#include "sprite.h"

namespace Shooter {
	// デフォルトでenabledにfalseを設定するため、生成しただけでは更新されない。更新対象に加えるにはSpawnedを実行する。
	class Mover : public GameObject
	{
	public:
		Mover(const Vector2& position, const float speed,
				const float angle, const std::shared_ptr<Sprite> sprite)
			: GameObject(false, position)
			, speed(speed)
			, angle(angle)
			, sprite(sprite) {}
		virtual ~Mover() {}
		virtual void Draw() override;
		void Spawned(const float speed, const float angle);
		void Spawned();
		virtual void Update() override;

		float GetSpeed() const
		{
			return speed;
		}

		void SetSpeed(const float speed)
		{
			this->speed = speed;
		}

		float GetAngle() const
		{
			return angle;
		}

		void SetAngle(const float angle)
		{
			this->angle = std::fmod(angle, 2 * M_PI);
		}
	protected:
		float speed;  // 単位：ドット毎フレーム
		float angle;  // x軸から時計回りに回転したときの角度。
		std::shared_ptr<Sprite> sprite;
		bool isInvisibleFor1Sec() const;
	};

	class Player : public Mover
	{
	public:
		static const int Height = 48;
		static const int Width = 32;
		Player(const Vector2& position, const float highSpeed, const float lowSpeed, const std::shared_ptr<Sprite> sprite);
		~Player() = default;
		void Draw() override;
		void Update() override;
	private:
		std::array<std::array<SDL_Rect, 5>, 3> clips;  // 3成分はそれぞれ停止時、左移動、右移動を表す。5成分は変化の差分を表す。
		float lowSpeed;
		Vector2 velocity;  // 自機はxy座標系の方が扱いやすい。
		void move();
	};

	class PlayerManager : public ObjectManager
	{
	public:
		enum class PlayerID
		{
			Reimu,
			Marisa,
			Sanae
		};

		std::shared_ptr<Player> GenerateObject(const PlayerID id, const Vector2& position);
	};

	class Enemy : public Mover
	{
	public:
		static const int Height = 32;
		static const int Width = 32;
		Enemy(const Vector2& position, const std::shared_ptr<Sprite> sprite);
		~Enemy() = default;
		void Draw() override;
		virtual void InitializeClips() = 0;
		void Update() override;
	protected:
		std::array<std::array<SDL_Rect, 3>, 3> clips;  // 具体的な値は継承先で設定せよ。
	};

	class EnemyManager : public ObjectManager
	{
	public:
		enum class EnemyID
		{
			SmallBlue
		};

		std::shared_ptr<Enemy> GenerateObject(const EnemyID id, const Vector2& position);
	};
}

#endif // !MOVER_H
