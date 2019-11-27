#include "bullet.h"
#include "media.h"
#include "player.h"

using namespace Shooter;

/******************************** 個別設定用クラス *********************************/

class Reimu : public Player
{
public:
	Reimu(const Vector2<float>& position)
		: Player(position, 4.5f, 2.0f, std::make_unique<Sprite>(AssetLoader::Create().GetTexture("images/Reimudot.png")), std::make_unique<CircleCollider>(1.0f), EffectManager::EffectID::DefetedPlayer)
	{}
};

class Marisa : public Player
{
public:
	Marisa(const Vector2<float>& position)
		: Player(position, 5.0f, 2.0f, std::make_unique<Sprite>(AssetLoader::Create().GetTexture("images/Marisadot.png")), std::make_unique<CircleCollider>(1.3f), EffectManager::EffectID::DefetedPlayer)
	{}
};

class Sanae : public Player
{
public:
	Sanae(const Vector2<float>& position)
		: Player(position, 4.5f, 2.0f, std::make_unique<Sprite>(AssetLoader::Create().GetTexture("images/Sanaedot.png")), std::make_unique<CircleCollider>(1.3f), EffectManager::EffectID::DefetedPlayer)
	{}
};

/******************************** Player *********************************/

/// <param name="position">初期位置</param>
/// <param name="highSpeed">高速移動時の速さ（単位：ドット毎フレーム）</param>
/// <param name="lowSpeed">低速移動時の速さ（単位：ドット毎フレーム）</param>
/// <param name="sprite">Spriteクラスへのポインタ（画像はAssetLoaderから指定）</param>
/// <param name="collider">当たり判定クラスへのポインタ</param>
/// <param name="effectID">消滅エフェクトのID</param>
Player::Player(const Vector2<float>& position, const float highSpeed, const float lowSpeed, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID)
	: Mover(position, 0.0f, -M_PI_2, std::move(sprite), std::move(collider), effectID, 1, 0)
	, highSpeed(highSpeed)
	, lowSpeed(lowSpeed)
{
	for (int i = 0; i < static_cast<int>(clips.size()); i++)
		for (int j = 0; j < static_cast<int>(clips[i].size()); j++)
			clips[i][j] = { j * Width, i * Height, Width, Height };
	this->sprite->SetClip(clips[0][0]);  // 最初のフレームだけ画像全体が表示されてしまうため、ここで明示的に代入。
}

void Player::Update()
{
	Mover::Update();

	// 移動制限。ただし、速度を変化させた場合のみで、位置を直接変える場合は制限しない。
	if (position.x - Width * 0.5f < 0 || position.x + Width * 0.5f > Media::Create().GetWidth())
		position.x -= velocity.x;
	if (position.y - Height * 0.5f < 0 || position.y + Height * 0.5f > Media::Create().GetHeight())
		position.y -= velocity.y;
	/* 復帰処理との兼ね合い（画面外から移動させる）から、次のようには書かない。
	if (position.x - Width * 0.5f < 0)
		position.x = Width * 0.5f;
	else if (position.x + Width * 0.5f > Media::Create().GetWidth())
		position.x = Media::Create().GetWidth() - Width * 0.5f;
	if (position.y - Height * 0.5f < 0)
		position.y = Height * 0.5f;
	else if (position.y + Height * 0.5f > Media::Create().GetHeight())
		position.y = Media::Create().GetHeight() - Height * 0.5f;*/

	// 描画の前処理。
	if (sprite->GetAlpha() < 255) {
		if (invincibleCounter == 0)
			sprite->SetAlpha(255);
		else if (invincibleCounter / 3 % 2 == 0)  // 3フレーム毎に点滅する。
			sprite->SetAlpha(0);
		else
			sprite->SetAlpha(191);
	}
}

void Player::OnCollide(Mover&)
{
	if (invincibleCounter > 0)
		return;
	enabled = false;
	hitPoint = 0;
	--life;
}

void Player::Spawned()
{
	if (life <= 0)
		return;
	Mover::spawned();
	SetSpeed(0.0f);
	SetAngle(-M_PI_2);
	velocity = { 0.0f, 0.0f };
	sprite->SetAlpha(191);
}

Rect<int>& Player::clipFromImage(unsigned int countedFrames)
{
	const int DelayFrames = 3;  // 左右移動の際に次の画像に切り替わるまでのフレーム数。
	const int NumSlice = 5;     // 停止時、左移動、右移動における各変化のコマ数。
	const int Period = 6;       // 停止時などに画像を繰り返す周期。
	static int level = 0;       // 左右に何フレーム進んでいるか表すフラグ。-(DelayFrames * NumSlice - 1) .. DelayFrames * NumSlice - 1 の範囲を動く。
	if (velocity.x < 0.0f)
		level = std::max(level - 1, -(DelayFrames * NumSlice - 1));
	else if (velocity.x > 0.0f)
		level = std::min(level + 1, DelayFrames * NumSlice - 1);
	else
		level = (level != 0) ? (level - level / std::abs(level)) : 0;

	if (level == 0)
		return clips[0][(countedFrames / Period) % NumSlice];
	else if (level == -(DelayFrames * NumSlice - 1))  // 4コマ目と5コマ目だけ繰り返し。
		return clips[1][(countedFrames / Period) % 2 + 3];
	else if (level == DelayFrames * NumSlice - 1)
		return clips[2][(countedFrames / Period) % 2 + 3];
	else if (level < 0)
		return clips[1][-level / DelayFrames];
	else
		return clips[2][level / DelayFrames];
}

/******************************** PlayerManager *********************************/

std::weak_ptr<Player> PlayerManager::GenerateObject(const PlayerID id, const Vector2<float>& position)
{
	std::weak_ptr<Player> newObject;
	switch (id) {
	case PlayerID::Reimu:
		newObject = assignObject<Reimu>(position);
		break;
	case PlayerID::Marisa:
		newObject = assignObject<Marisa>(position);
		break;
	case PlayerID::Sanae:
		newObject = assignObject<Sanae>(position);
		break;
	}
	return newObject;
}

std::weak_ptr<Player> PlayerManager::GetPlayer() const
{
	for (auto&& object : objectsList) {
		auto temp = std::dynamic_pointer_cast<Player>(object);
		if (temp.get() != nullptr)  // インスタンスの親子関係を調べる。もっとよい方法はないのか？
			return temp;
	}
	return {};
}
