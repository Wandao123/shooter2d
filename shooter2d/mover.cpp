﻿#include <algorithm>
#include "game.h"
#include "mover.h"

using namespace Shooter;

void Mover::Draw()
{
	SDL_Rect& currentClip = clipFromImage(Time->GetCountedFrames());
	sprite->SetClip(currentClip);
	sprite->Draw(position);
}

void Mover::spawned()
{
	enabled = true;
	counter = 0;
}

void Mover::Update()
{
	position += { speed * std::cos(angle), speed * std::sin(angle) };  // 1フレームを単位時間とする。
	if (!isInside())
		enabled = false;
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