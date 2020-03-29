#ifndef PLAYER_H
#define PLAYER_H

#include <array>
#include "mover.h"

namespace Shooter {
	/// <summary>自機クラス。キャラクターとオプションとに分かれる。</summary>
	class Player : public Mover
	{
	public:
		bool SlowMode = false;  // 低速移動か否か。
		Player(const Vector2<double>& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider);
		virtual void Update() override;
		void OnCollide(Mover& mover) override;
		void Spawned();  // 実体化関数

		const Vector2<double>& GetVelocity() const&
		{
			return velocity;
		}

		/// <summary>速度の方向を設定する。入力との関係上、speedとangle（極座標系）ではなくxy座標系で指定する。</summary>
		/// <param name="velocity">xy座標系での速度</param>
		virtual void SetVelocity(const Vector2<double>& velocity)
		{
			//this->velocity = velocity * Timer::FPS * Timer::Create().GetDeltaTime();  // 実時間を考慮する場合。
			this->velocity = velocity;
			SetSpeed(this->velocity.Magnitude());
			SetAngle(std::atan2(this->velocity.y, this->velocity.x));
		}
	private:
		Vector2<double> velocity = { 0.0, 0.0 };
	};

	class PlayerManager : public ObjectManager//, public std::enable_shared_from_this<PlayerManager>
	{
	public:
		enum class PlayerID
		{
			// 自機。
			Reimu,
			Marisa,
			Sanae,
			// オプション。
			ReimuOption,
			MarisaOption,
			SanaeOption
		};
		const Vector2<int> CharacterSize;

		PlayerManager();
		std::weak_ptr<Player> GenerateObject(const PlayerID id, const Vector2<double>& position);
		std::weak_ptr<Player> GetPlayer() const;  // TODO: objectsListに複数のアイテムが入っている場合を考慮。
	};
}

#endif // !PLAYER_H
