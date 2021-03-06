#include "mover.h"
#include "media.h"
#include <algorithm>

using namespace Shooter;

void Mover::Draw() const
{
	sprite->Draw(position);
}

void Mover::Update()
{
	// パラメータの更新。
	position += { speed * std::cos(angle), speed * std::sin(angle) };  // 1フレームを単位時間とする。
	++counter;
	if (!isInside())
		enabled = false;
	invincibleCounter = (invincibleCounter > 0) ? invincibleCounter - 1 : 0;

	// アニメーション。
	Rect<int>& currentClip = clipFromImage(Timer::Create().GetPlayingFrames());
	sprite->SetClip(currentClip);
}

/// <summary>オブジェクトが画面内に存在するか？</summary>
/// <returns>存在すれば真、しなければ偽</returns>
/// <remarks>
/// 生成されてから最初の1秒（60フレーム）は判定せずに真を返す。画面外でも0.1秒（6フレーム）以内に画面内に戻れば真を返す。なお、ここでいう「存在」とはオブジェクトの画像の一部でも画面内にあることを意味する。
/// </remarks>
bool Mover::isInside()
{
	if (counter < 60)
		return true;
	if (position.x + sprite->GetClip().width * 0.5 < 0 || position.x - sprite->GetClip().width * 0.5 > Media::Create().GetWidth()
		|| position.y + sprite->GetClip().height * 0.5 < 0 || position.y - sprite->GetClip().height * 0.5 > Media::Create().GetHeight()) {
		if (counter > 66)
			return false;
		else
			return true;
	} else {
		counter = 60;
		return true;
	}
}

void Mover::spawned()
{
	enabled = true;
	hitPoint = 1;
	counter = 0;
}
