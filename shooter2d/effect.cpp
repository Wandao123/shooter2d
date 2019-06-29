#include "effect.h"

using namespace Shooter;

/******************************** 個別設定用クラス *********************************/

/// <summary>何もしないエフェクト</summary>
class NoneEffect : public Effect
{
public:
	NoneEffect(const Vector2& position)
		: Effect(position, std::make_unique<Sprite>("images/effect_circle.png"))
	{}

	void Played() override {}
};

/// <summary>放射状に広がる青いエフェクト</summary>
class BlueCircleEffect : public Effect
{
public:
	BlueCircleEffect(const Vector2& position)
		: Effect(position, std::make_unique<Sprite>("images/effect_circle.png"))
	{
		clips[0] = { 128, 128, 128, 128 };
		clips[1] = { 128, 128, 128, 128 };
		clips[2] = { 128, 256, 128, 128 };
	}
};

/******************************** Effect *********************************/

Effect::Effect(const Vector2& position, std::unique_ptr<Sprite>&& sprite)
	: GameObject(false, position)
	, sprite(std::move(sprite))
{}

void Effect::Draw()
{
	auto clipFromImage = [this]() -> SDL_Rect& {
		if (static_cast<float>(counter) / AnimationFrams < 0.3f)
			return clips[1];
		else
			return clips[2];
	};

	SDL_Rect& currentClip = clipFromImage();
	sprite->SetClip(currentClip);
	sprite->SetAlpha(alpha);
	sprite->Draw(position, 0.0f, scalingRate);
}

void Effect::Played()
{
	enabled = true;
	counter = 0;
	alpha = 255;
	scalingRate = 0.0f;
}

void Effect::Update()
{
	if (counter < AnimationFrams) {
		alpha = 255 - 255 * counter / AnimationFrams;
		scalingRate = 0.5f + counter * 0.05f;
		++counter;
	}  else {
		enabled = false;
	}
}

/******************************** EffectManager *********************************/

std::weak_ptr<Effect> EffectManager::GenerateObject(const EffectID id, const Vector2& position)
{
	std::weak_ptr<Effect> newObject;
	switch (id) {
	case EffectID::None:
		newObject = assignObject<NoneEffect>(position);
		break;
	case EffectID::BlueCircle:
		newObject = assignObject<BlueCircleEffect>(position);
		break;
	}
	return newObject;
}