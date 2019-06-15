#ifndef MOVER_H
#define MOVER_H

#include "collider.h"
#include "effect.h"

namespace Shooter {
	// デフォルトでenabledにfalseを設定するため、生成しただけでは更新されない。
	// 更新対象に加えるには「実体化関数」（characterは ``Spawned''、bullet は ``Shot''、effect は ``Played''）を実行する。分けたのは、各オブジェクトで渡すべき引数が異なるため。
	class Mover : public GameObject
	{
	public:
		Mover(const Vector2& position, const float speed, const float angle, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID, const unsigned int damage)
			: GameObject(false, position)
			, speed(speed)
			, angle(angle)
			, damage(damage)
			, sprite(std::move(sprite))
			, collider(std::move(collider))
			, effectID(effectID)
		{}
		virtual ~Mover() {}
		virtual void Draw() override;
		virtual void Update() override;
		virtual void OnCollide(Mover& mover) = 0;

		float GetSpeed() const
		{
			return speed;
		}

		void SetSpeed(const float speed)
		{
			if (enabled)
				this->speed = speed;
		}

		float GetAngle() const
		{
			return angle;
		}

		void SetAngle(const float angle)
		{
			if (enabled)
				this->angle = std::fmod(angle, 2 * M_PI);
		}

		Collider& GetCollider() const
		{
			return *collider;
		}

		EffectManager::EffectID GetEffectID() const
		{
			return effectID;
		}

		unsigned int GetDamage() const
		{
			return damage;
		}
	protected:
		float speed;  // 単位：ドット毎フレーム
		float angle;  // x軸から時計回りに回転したときの角度。
		unsigned int damage;  // 衝突時に相手に与えるダメージ。
		std::unique_ptr<Sprite> sprite;
		std::unique_ptr<Collider> collider;
		EffectManager::EffectID effectID;  // 消滅エフェクトのID。
		unsigned int counter = 0;  // enabledがtrueになってからのフレーム数。
		bool isInside();
		virtual void Spawned();
	};

	class Bullet : public Mover
	{
	public:
		Bullet(const Vector2& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID, unsigned int damage);
		void Draw() override;
		void Update() override;
		void OnCollide(Mover& mover) override;
		void Shot(const float speed, const float angle);
	};

	class BulletManager : public ObjectManager
	{
	public:
		enum class BulletID
		{
			Small
		};

		std::shared_ptr<Bullet> GenerateObject(const BulletID id, const Vector2& position);
	};

	class PlayerManager;

	class Player : public Mover
	{
	public:
		static const int Height = 48;
		static const int Width = 32;
		Player(const Vector2& position, const float highSpeed, const float lowSpeed, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID);
		~Player() = default;
		void Draw() override;
		void Update() override;
		void OnCollide(Mover& mover) override;

		void SetManager(std::shared_ptr<PlayerManager> manager)
		{
			this->manager = manager;
		}

		void Spawned() override
		{
			Mover::Spawned();
		}
	private:
		std::array<std::array<SDL_Rect, 5>, 3> clips;  // 3成分はそれぞれ停止時、左移動、右移動を表す。5成分は変化の差分を表す。
		float lowSpeed;
		Vector2 velocity;  // 自機はxy座標系の方が扱いやすい。
		std::weak_ptr<PlayerManager> manager;
		void move();
		void shoot();
	};

	// このクラスではプレイヤーオブジェクトとプレイヤーの弾オブジェクトの両方を管理する。プレイヤーオブジェクトからの指示に従って弾を生成したいため。
	class PlayerManager : public ObjectManager, public std::enable_shared_from_this<PlayerManager>
	{
	public:
		enum class PlayerID
		{
			Reimu,
			Marisa,
			Sanae
		};
		enum class BulletID
		{
			ReimuNormal
		};

		std::shared_ptr<Player> GenerateObject(const PlayerID id, const Vector2& position);
		std::shared_ptr<Bullet> GenerateObject(const BulletID id, const Vector2& position);
		std::shared_ptr<Player> GetPlayer() const;
	};

	class Enemy : public Mover
	{
	public:
		static const int Height = 32;
		static const int Width = 32;
		Enemy(const Vector2& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID);
		~Enemy() = default;
		void Draw() override;
		void Update() override;
		void OnCollide(Mover& mover) override;
		void Spawned(const float speed, const float angle, const int hitPoint);
	protected:
		std::array<std::array<SDL_Rect, 3>, 3> clips;  // 具体的な値は継承先で設定せよ。
		int hitPoint;
		bool isDamaged = false;  // 被弾したか否か。
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
