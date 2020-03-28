#ifndef PLAYER_H
#define PLAYER_H

#include <array>
#include "mover.h"

namespace Shooter {
	/// <summary>自機クラス。</summary>
	class Player : public Mover
	{
	public:
		static const int Height = 48;
		static const int Width = 32;
		Player(const Vector2<double>& position, const double highSpeed, const double lowSpeed, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider);
		void Update() override;
		void OnCollide(Mover& mover) override;
		void Spawned();  // 実体化関数

		/// <summary>速度の方向を設定する。入力との関係上、speedとangle（極座標系）ではなくxy座標系で指定する。</summary>
		/// <param name="direction">xy座標系での移動方向</param>
		/// <param name="slowMode">低速移動モードか否か</param>
		/// <remarks>速度の方向のみ考慮することに注意（大きさは規格化される）。速さは予め設定されたhighSpeed/lowSpeedを使う。</remarks>
		void SetVelocity(const Vector2<double>& direction, const bool slowMode)
		{
			auto speed = slowMode ? lowSpeed : highSpeed;
			//this->velocity = direction.Normalize() * speed * Timer::FPS * Timer::Create().GetDeltaTime();  // 実時間を考慮する場合。
			this->velocity = direction.Normalize() * speed;
			SetSpeed(this->velocity.Magnitude());
			SetAngle(std::atan2(this->velocity.y, this->velocity.x));
		}
	protected:
		Rect<int>& clipFromImage(unsigned int countedFrames) override;
	private:
		std::array<std::array<Rect<int>, 5>, 3> clips;  // 3成分はそれぞれ停止時、左移動、右移動を表す。5成分は変化の差分を表す。
		const double highSpeed;
		const double lowSpeed;
		Vector2<double> velocity = { 0.0, 0.0 };
	};

	/// <summary>自機オプションクラス。</summary>
	/// <remarks>現在の実装では当たり判定は無い。</remarks>
	class PlayerOption : public Mover
	{
	public:
		PlayerOption(const Vector2<double>& position, std::unique_ptr<Sprite>&& sprite);
		void OnCollide(Mover&) override {}
		void Spawned();  // 実体化関数
	protected:
		Rect<int>& clipFromImage(unsigned int countedFrames) override;
	private:
		Rect<int> clip;
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
			// オプション。PlayerOptionクラスをPlayerクラス自体が持つようにすれば不要かもしれない。
			//ReimuOption,
			//MarisaOption,
			//SanaeOption
		};

		PlayerManager();
		std::weak_ptr<Player> GenerateObject(const PlayerID id, const Vector2<double>& position);
		std::weak_ptr<Player> GetPlayer() const;  // TODO: objectsListに複数のアイテムが入っている場合を考慮。
	};
}

#endif // !PLAYER_H
