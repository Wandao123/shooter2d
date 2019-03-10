#ifndef MOVER_H
#define MOVER_H

#include <array>
#include "game_object.h"
#include "sprite.h"
#include "timer.h"

namespace Shooter {
	extern std::unique_ptr<Timer> Time;
	extern std::unique_ptr<AssetLoader> assetLoader;

	class Mover : public GameObject
	{
	public:
		Mover(const Vector2 &position, const std::shared_ptr<Sprite> sprite) : GameObject(position), sprite(sprite) {}
		// TODO: 仮想デストラクタが必須？
		virtual ~Mover() {}
		virtual void Draw() override;

		void SetSprite(const std::shared_ptr<Sprite> sprite)
		{
			this->sprite = sprite;
		}

		// HACK: 参照を返したほうがいい？
		Vector2 GetVelocity() const
		{
			return velocity;
		}
	protected:
		std::shared_ptr<Sprite> sprite;
		Vector2 velocity = { 0.0f, 0.0f };
	};

	// TODO: 後々、抽象クラスにして機体の種類毎にクラスを分ける。
	class Player : public Mover
	{
	public:
		static const int Height = 48;
		static const int Width = 32;
		Player(const Vector2 &position, const float highSpeed, const float lowSpeed);
		~Player() = default;
		void Draw() override;
		void Update() override;
	private:
		std::array<std::array<SDL_Rect, 5>, 3> clips;
		float highSpeed;  // 単位：ドット毎フレーム
		float lowSpeed;
		// TODO: 回転角もプロパティとして持つ。
		void move();
	};
}

#endif // !MOVER_H
