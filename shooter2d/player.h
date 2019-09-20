#ifndef PLAYER_H
#define PLAYER_H

#include "mover.h"

namespace Shooter {
	class PlayerManager;

	/// <summary>自機クラス。</summary>
	class Player : public Mover
	{
	public:
		static const int Height = 48;
		static const int Width = 32;
		Player(const Vector2& position, const float highSpeed, const float lowSpeed, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID);
		~Player() = default;
		void Update() override;
		void OnCollide(Mover& mover) override;
		virtual void Shoot() = 0;
		void Spawned();  // 実体化関数

		float GetHighSpeed() const
		{
			return highSpeed;
		}

		float GetLowSpeed() const
		{
			return lowSpeed;
		}

		int GetLife() const
		{
			return life;
		}

		/// <param name="life">ゲームオーバーまでの被弾数</param>
		void SetLife(const int life)
		{
			if (life > 0)
				this->life = life;
		}

		/// <summary>速度のセッター。入力との関係上、speedとangleではなくxy座標で指定したい。</summary>
		/// <param name="velocity">xy座標系での移動速度（単位：ドット毎フレーム）</param>
		void SetVelocity(const Vector2& velocity)
		{
			//this->velocity = velocity * Timer::FPS * Timer::Create().GetDeltaTime();  // 実時間を考慮する場合。
			this->velocity = velocity;
			SetSpeed(this->velocity.Magnitude());
			SetAngle(std::atan2(this->velocity.y, this->velocity.x));
		}
	protected:
		std::weak_ptr<PlayerManager> manager;
		SDL_Rect& clipFromImage(unsigned int countedFrames) override;
	private:
		friend class PlayerManager;
		std::array<std::array<SDL_Rect, 5>, 3> clips;  // 3成分はそれぞれ停止時、左移動、右移動を表す。5成分は変化の差分を表す。
		const float highSpeed;
		const float lowSpeed;
		int life = 3;  // 1まではプレイ可能で、0になったらゲームオーバー。life - 1 = 残機。
		Vector2 velocity = { 0.0f, 0.0f };

		/// <summary>相互参照用のセッター。</summary>
		/// <remarks>PlayerManager::GenerateObjectの引数を変えないために、後から代入できるようにする。</remarks>
		void setManager(std::shared_ptr<PlayerManager> manager)
		{
			this->manager = manager;
		}
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
			ReimuNormal,
			MarisaNormal,
			SanaeNormal
		};

		std::weak_ptr<Player> GenerateObject(const PlayerID id, const Vector2& position);
		std::weak_ptr<Bullet> GenerateObject(const BulletID id, const Vector2& position);
		std::weak_ptr<Player> GetPlayer() const;
	};
}

#endif // !PLAYER_H
