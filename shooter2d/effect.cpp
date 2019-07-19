#include "effect.h"

using namespace Shooter;

/******************************** 個別設定用クラス *********************************/

/// <summary>何もしないエフェクト</summary>
class NoneEffect : public Effect
{
public:
	NoneEffect(const Vector2& position)
		: Effect(position, std::make_unique<Sprite>("images/effect_circle.png"), std::make_unique<Sound>("se/enemy_vanish_effect-A.wav", Sound::Mode::Chunk))
	{}

	void Played() override {}
};

class DefetedPlayerEffect : public Effect
{
public:
	DefetedPlayerEffect(const Vector2& position)
		: Effect(position, std::make_unique<Sprite>("images/effect_circle.png"), std::make_unique<Sound>("se/nc899.wav", Sound::Mode::Chunk))
	{
		clips[0] = { 0, 128, 128, 128 };
		clips[1] = { 0, 128, 128, 128 };
		clips[2] = { 0, 256, 128, 128 };
		this->sound->SetVolume(Sound::MaxVolume / 4);
	}

	void Update() override
	{
		if (counter < AnimationFrames) {
			alpha = 255 - 255 * counter / AnimationFrames;
			scalingRate = 0.5f + counter * 0.1f;
			++counter;
		}
		else {
			enabled = false;
		}
		Effect::Update();
	}
};

/// <summary>円形に広がるエフェクト</summary>
class CircleEffect : public Effect
{
public:
	CircleEffect(const Vector2& position, std::unique_ptr<Sprite>&& sprite)
		: Effect(position, std::move(sprite), std::make_unique<Sound>("se/enemy_vanish_effect-A.wav", Sound::Mode::Chunk))
	{
		this->sound->SetVolume(Sound::MaxVolume / 2);
	}

	void Update() override
	{
		if (counter < AnimationFrames) {
			alpha = 255 - 255 * counter / AnimationFrames;
			scalingRate = 0.5f + counter * 0.05f;
			++counter;
		}
		else {
			enabled = false;
		}
		Effect::Update();
	}
};

class RedCircleEffect : public CircleEffect
{
public:
	RedCircleEffect(const Vector2& position)
		: CircleEffect(position, std::make_unique<Sprite>("images/effect_circle.png"))
	{
		clips[0] = { 0, 128, 128, 128 };
		clips[1] = { 0, 128, 128, 128 };
		clips[2] = { 0, 256, 128, 128 };
	}
};

class BlueCircleEffect : public CircleEffect
{
public:
	BlueCircleEffect(const Vector2& position)
		: CircleEffect(position, std::make_unique<Sprite>("images/effect_circle.png"))
	{
		clips[0] = { 128, 128, 128, 128 };
		clips[1] = { 128, 128, 128, 128 };
		clips[2] = { 128, 256, 128, 128 };
	}
};

/******************************** Effect *********************************/

Effect::Effect(const Vector2& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Sound>&& sound)
	: GameObject(false, position)
	, sprite(std::move(sprite))
	, sound(std::move(sound))
{}

void Effect::Draw() const
{
	sprite->Draw(position, 0.0f, scalingRate);
}

void Effect::Played()
{
	enabled = true;
	counter = 0;
	alpha = 255;
	scalingRate = 0.0f;
	sound->Played();
}

void Effect::Update()
{
	auto clipFromImage = [this]() -> SDL_Rect& {
		if (static_cast<float>(counter) / AnimationFrames < 0.3f)
			return clips[1];
		else
			return clips[2];
	};
	SDL_Rect& currentClip = clipFromImage();
	sprite->SetClip(currentClip);
	sprite->SetAlpha(alpha);
}

/******************************** EffectManager *********************************/

std::weak_ptr<Effect> EffectManager::GenerateObject(const EffectID id, const Vector2& position)
{
	std::weak_ptr<Effect> newObject;
	switch (id) {
	case EffectID::None:
		newObject = assignObject<NoneEffect>(position);
		break;
	case EffectID::DefetedPlayer:
		newObject = assignObject<DefetedPlayerEffect>(position);
		break;
	case EffectID::RedCircle:
		newObject = assignObject<RedCircleEffect>(position);
		break;
	case EffectID::BlueCircle:
		newObject = assignObject<BlueCircleEffect>(position);
		break;
	}
	return newObject;
}
