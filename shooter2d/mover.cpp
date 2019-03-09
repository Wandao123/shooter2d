#include <algorithm>
#include "mover.h"

using namespace Shooter;

Player::Player(const std::string &path, const float highSpeed, const float lowSpeed)
	: Mover(path)
	, highSpeed(highSpeed)
	, lowSpeed(lowSpeed)
{
	// HACK: マジックナンバーの削除。
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 5; j++)
			clips[i][j] = { j * ImgWidth, i * ImgHeight, ImgWidth, ImgHeight };
}

void Player::Draw()
{
	// HACK: ここはファイル構造に強く依存するため、アルゴリズムだけ巧く抽出できないか？
	// 少なくとも6フレームは同じ画像を表示。アニメーションは5コマ（4コマ目と5コマ目は繰り返し）ある。
	auto clipFromImage = [this](Uint32 countedFrames) -> SDL_Rect* {
		const int DelayFrames = 6;
		const int NumSlice = 5;
		static int level = 0;  // 左右に何フレーム進んでいるか表すフラグ。-(6 * 5 - 1) .. 6 * 5 - 1 の範囲を動く。
		if (velocity.x < 0.0f)
			level = std::max(level - 1, -(DelayFrames * NumSlice - 1));
			//level -= (level > -(DelayFrames * NumSlice - 1)) ? 1 : 0;
		else if (velocity.x > 0.0f)
			level = std::min(level + 1, DelayFrames * NumSlice - 1);
			//level += (level < DelayFrames * NumSlice - 1) ? 1 : 0;
		else
			level = (level != 0) ? (level - level / std::abs(level)) : 0;

		if (level == 0)
			return &clips[0][(countedFrames / DelayFrames) % NumSlice];
		else if (level == -(DelayFrames * NumSlice - 1))  // 4コマ目と5コマ目だけ繰り返し。
			return &clips[1][(countedFrames / DelayFrames) % 2 + 3];
		else if (level == DelayFrames * NumSlice - 1)
			return &clips[2][(countedFrames / DelayFrames) % 2 + 3];
		else if (level < 0)
			return &clips[1][-level / DelayFrames];
		else
			return &clips[2][level / DelayFrames];
	};

	SDL_Rect *currentClip = clipFromImage(Time->GetCountedFrames());
	SDL_Rect renderClip = { static_cast<int>(position.x), static_cast<int>(position.y), currentClip->w, currentClip->h };
	SDL_RenderCopy(Renderer, texture, currentClip, &renderClip);
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
	Move();
}

void Player::Move()
{
	position += velocity * Time->GetDeltaTime();
	if ((position.x < 0) || (position.x + ImgWidth > ScreenWidth))
		position.x -= velocity.x * Time->GetDeltaTime();
	if ((position.y < 0) || (position.y + ImgHeight > ScreenHeight))
		position.y -= velocity.y * Time->GetDeltaTime();
}