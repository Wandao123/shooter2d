#ifndef PLAYER_H
#define PLAYER_H

#include "mover.h"

namespace Shooter {
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

		/// <param name="life">ゲームオーバーまでの被弾回数</param>
		void SetLife(const int life)
		{
			if (life > 0)
				this->life = life;
		}

		/// <summary>速度の方向を設定する。入力との関係上、speedとangle（極座標系）ではなくxy座標系で指定する。</summary>
		/// <param name="velocity">xy座標系での移動速度（単位：ドット毎フレーム）</param>
		void SetVelocity(const Vector2& velocity)
		{
			//this->velocity = velocity * Timer::FPS * Timer::Create().GetDeltaTime();  // 実時間を考慮する場合。
			this->velocity = velocity;
			SetSpeed(this->velocity.Magnitude());
			SetAngle(std::atan2(this->velocity.y, this->velocity.x));
		}
	protected:
		SDL_Rect& clipFromImage(unsigned int countedFrames) override;
	private:
		std::array<std::array<SDL_Rect, 5>, 3> clips;  // 3成分はそれぞれ停止時、左移動、右移動を表す。5成分は変化の差分を表す。
		const float highSpeed;
		const float lowSpeed;
		int life = 3;  // 1まではプレイ可能で、0になったらゲームオーバー。life - 1 = 残機。hitPointは撃破の判定に用いるのに対し、これは被弾回数を表す。
		Vector2 velocity = { 0.0f, 0.0f };
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

		std::weak_ptr<Player> GenerateObject(const PlayerID id, const Vector2& position);
		std::weak_ptr<Player> GetPlayer() const;  // TODO: objectsListに複数のアイテムが入っている場合を考慮。
	};
}

#endif // !PLAYER_H
