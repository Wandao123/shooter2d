#include <algorithm>
#include "game.h"
#include "mover.h"

using namespace Shooter;

void Mover::Draw() const
{
	sprite->Draw(position);
}

void Mover::Update()
{
	// 移動処理。
	position += { speed * std::cos(angle), speed * std::sin(angle) };  // 1フレームを単位時間とする。
	if (!isInside())
		enabled = false;

	// アニメーション。
	SDL_Rect& currentClip = clipFromImage(Time->GetCountedFrames());
	sprite->SetClip(currentClip);
}

/// <summary>オブジェクトが画面内に存在するか？</summary>
/// <returns>存在すれば真、しなければ偽</returns>
/// <remarks>生成されてから最初の1秒（60フレーム）は判定せずに真を返す。</remarks>
bool Mover::isInside()
{
	if (counter < 60) {
		++counter;
		return true;
	}
	if (position.x < -sprite->GetClip().w / 2 || position.x > Game::Width + sprite->GetClip().w / 2
		|| position.y < -sprite->GetClip().h / 2 || position.y > Game::Height + sprite->GetClip().h / 2)
		return false;
	else
		return true;
}

void Mover::spawned()
{
	enabled = true;
	hitPoint = 1;
	counter = 0;
}