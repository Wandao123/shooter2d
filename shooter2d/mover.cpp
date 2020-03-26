#include "mover.h"
#include "media.h"
#include <algorithm>

using namespace Shooter;


/// <param name="position">初期位置</param>
/// <param name="speed">初期速度の速さ</param>
/// <param name="angle">初期速度の方向</param>
/// <param name="sprite">Spriteクラスへのポインタ（画像はAssetLoaderから指定）</param>
/// <param name="collider">当たり判定クラスへのポインタ</param>
/// <param name="damage">衝突時に相手に与えるダメージ</param>
/// <param name="hitPoint">体力</param>
/// <param name="autoDisabling">画面外に出たら自動的に無効にするか否か</param>
Mover::Mover(const Vector2<double>& position, const double speed, const double angle, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, const unsigned int damage, const int hitPoint, bool autoDisabling)
	: GameObject(false, position)
	, speed(speed)
	, angle(angle)
	, damage(damage)
	, hitPoint(hitPoint)
	, sprite(std::move(sprite))
	, collider(std::move(collider))
	, invincibleCounter(0)
	, existingCounter(0)
{
	if (autoDisabling)
		disableIfOutside = [this] {
			++existingCounter;
			if (!isInside())
				enabled = false;
		};
	else
		disableIfOutside = [] {};
}

void Mover::Draw() const
{
	sprite->Draw(position);
}

void Mover::Update()
{
	// パラメータの更新。
	position += { speed * std::cos(angle), speed * std::sin(angle) };  // 1フレームを単位時間とする。
	invincibleCounter = (invincibleCounter > 0) ? invincibleCounter - 1 : 0;
	disableIfOutside();

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
	if (existingCounter < 60)
		return true;
	if (position.x + sprite->GetClip().width * 0.5 < 0 || position.x - sprite->GetClip().width * 0.5 > Media::Create().GetWidth()
			|| position.y + sprite->GetClip().height * 0.5 < 0 || position.y - sprite->GetClip().height * 0.5 > Media::Create().GetHeight()) {
		if (existingCounter > 66)
			return false;
		else
			return true;
	} else {
		existingCounter = 60;
		return true;
	}
}

void Mover::spawned()
{
	enabled = true;
	hitPoint = 1;
	existingCounter = 0;
}
