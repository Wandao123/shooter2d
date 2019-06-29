#ifndef EFFECT_H
#define EFFECT_H

#include <array>
#include "game_object.h"
#include "assets.h"

namespace Shooter {
	class Effect : public GameObject
	{
	public:
		Effect(const Vector2& position, std::unique_ptr<Sprite>&& sprite);
		virtual ~Effect() {}
		void Draw() override;
		virtual void Played();
		void Update() override;
	protected:
		std::array<SDL_Rect, 3> clips;  // 具体的な値は継承先で設定せよ。
	private:
		const unsigned int AnimationFrams = 30;
		std::unique_ptr<Sprite> sprite;
		unsigned int counter = 0;  // エフェクトのアニメーションが起きているフレーム数。アニメーションが進むと減少する。
		unsigned int alpha = 255;
		float scalingRate = 0.0f;
	};

	class EffectManager : public ObjectManager
	{
	public:
		enum class EffectID
		{
			None,
			BlueCircle
		};

		std::weak_ptr<Effect> GenerateObject(const EffectID id, const Vector2& position);
	};
}

#endif // !EFFECT_H
