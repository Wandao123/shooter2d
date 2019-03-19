#include <algorithm>
#include "game.h"
#include "mover.h"

using namespace Shooter;

std::unique_ptr<AssetLoader> assetLoader = std::make_unique<AssetLoader>();

void Mover::Draw()
{
	sprite->Draw(static_cast<int>(position.x), static_cast<int>(position.y));
}

Player::Player(const Vector2 position, const float highSpeed, const float lowSpeed)
	: Mover(position, { 0.0f, 0.0f }, 0.0f, std::make_shared<Shooter::Sprite>(assetLoader->GetTexture("images/Reimudot.png")))
	, highSpeed(highSpeed)
	, lowSpeed(lowSpeed)
{
	// HACK: マジックナンバーの削除。
	for (int i = 0; i < clips.size(); i++)
		for (int j = 0; j < clips[i].size(); j++)
			clips[i][j] = { j * Width, i * Height, Width, Height };
}

void Player::Draw()
{
	// HACK: ここはファイル構造に強く依存するため、アルゴリズムだけ巧く抽出できないか？
	// 少なくとも6フレームは同じ画像を表示。アニメーションは5コマ（4コマ目と5コマ目は繰り返し）ある。
	auto clipFromImage = [this](Uint32 countedFrames) -> SDL_Rect& {
		const int DelayFrames = 6;
		const int NumSlice = 5;
		static int level = 0;  // 左右に何フレーム進んでいるか表すフラグ。-(DelayFrames * NumSlice - 1) .. DelayFrames * NumSlice - 1 の範囲を動く。
		if (velocity.x < 0.0f)
			level = std::max(level - 1, -(DelayFrames * NumSlice - 1));
		else if (velocity.x > 0.0f)
			level = std::min(level + 1, DelayFrames * NumSlice - 1);
		else
			level = (level != 0) ? (level - level / std::abs(level)) : 0;

		if (level == 0)
			return clips[0][(countedFrames / DelayFrames) % NumSlice];
		else if (level == -(DelayFrames * NumSlice - 1))  // 4コマ目と5コマ目だけ繰り返し。
			return clips[1][(countedFrames / DelayFrames) % 2 + 3];
		else if (level == DelayFrames * NumSlice - 1)
			return clips[2][(countedFrames / DelayFrames) % 2 + 3];
		else if (level < 0)
			return clips[1][-level / DelayFrames];
		else
			return clips[2][level / DelayFrames];
	};

	SDL_Rect& currentClip = clipFromImage(Time->GetCountedFrames());
	sprite->SetClip(std::make_shared<SDL_Rect>(currentClip));
	Mover::Draw();
}

void Player::Update()
{
	// TODO: キー入力処理の分離。
	float speed;  // 単位：ドット毎秒
	const SDL_Keymod modStates = SDL_GetModState();
	if (modStates & KMOD_SHIFT)
		speed = lowSpeed * Timer::FPS;
	else
		speed = highSpeed * Timer::FPS;
	velocity = { 0.0f, 0.0f };
	const Uint8 *currentKeyStates = SDL_GetKeyboardState(nullptr);
	if (currentKeyStates[SDL_SCANCODE_LEFT])
		velocity.x = -speed;
	if (currentKeyStates[SDL_SCANCODE_RIGHT])
		velocity.x = +speed;
	if (currentKeyStates[SDL_SCANCODE_UP])
		velocity.y = -speed;
	if (currentKeyStates[SDL_SCANCODE_DOWN])
		velocity.y = +speed;
	velocity = velocity.Normalize() * speed;
	move();
}

void Player::move()
{
	position += velocity * Time->GetDeltaTime();
	if ((position.x < 0) || (position.x + Width > Game::Width))
		position.x -= velocity.x * Time->GetDeltaTime();
	if ((position.y < 0) || (position.y + Height > Game::Height))
		position.y -= velocity.y * Time->GetDeltaTime();
}

Enemy::Enemy(const Vector2 position, const Vector2 velocity)
	: Mover(position, velocity, 0.0f, std::make_shared<Shooter::Sprite>(assetLoader->GetTexture("images/Enemy.png")))
{
	// HACK: マジックナンバーの削除。
	for (int j = 0; j < clips[0].size(); j++) {
		clips[0][j] = { j * Width, 0, Width, Height };
		clips[1][j] = { (j + 3) * Width, 2 * Height, Width, Height };
		clips[2][j] = { (j + 3) * Width, 0, Width, Height };
	}
}

void Enemy::Draw()
{
	// ここの部分はPlayerクラスと殆ど同じ。
	auto clipFromImage = [this](Uint32 countedFrames) -> SDL_Rect& {
		const int DelayFrames = 6;
		const int NumSlice = 3;
		static int level = 0;  // 左右に何フレーム進んでいるか表すフラグ。-(DelayFrames * NumSlice - 1) .. DelayFrames * NumSlice - 1 の範囲を動く。
		if (velocity.x < 0.0f)
			level = std::max(level - 1, -(DelayFrames * NumSlice - 1));
		else if (velocity.x > 0.0f)
			level = std::min(level + 1, DelayFrames * NumSlice - 1);
		else
			level = (level != 0) ? (level - level / std::abs(level)) : 0;

		if (level == 0)
			return clips[0][(countedFrames / DelayFrames) % NumSlice];
		else if (level < 0)
			return clips[1][-level / DelayFrames];
		else
			return clips[2][level / DelayFrames];
	};

	SDL_Rect& currentClip = clipFromImage(Time->GetCountedFrames());
	sprite->SetClip(std::make_shared<SDL_Rect>(currentClip));
	Mover::Draw();
}

void Enemy::Update()
{
	position += velocity;  // 1フレームを単位時間とする。
}
