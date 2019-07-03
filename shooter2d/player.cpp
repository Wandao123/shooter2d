#include "bullet.h"
#include "game.h"
#include "player.h"

using namespace Shooter;

/******************************** 個別設定用クラス *********************************/

class Reimu : public Player
{
public:
	Reimu(const Vector2& position)
		: Player(position, 4.5f, 2.0f, std::make_unique<Sprite>("images/Reimudot.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 1.0f), EffectManager::EffectID::RedCircle)
	{}
};

class Marisa : public Player
{
public:
	Marisa(const Vector2& position)
		: Player(position, 5.0f, 2.0f, std::make_unique<Sprite>("images/Marisadot.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 1.3f), EffectManager::EffectID::RedCircle)
	{}
};

class Sanae : public Player
{
public:
	Sanae(const Vector2& position)
		: Player(position, 4.5f, 2.0f, std::make_unique<Sprite>("images/Sanaedot.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 1.3f), EffectManager::EffectID::RedCircle)
	{}
};

class ReimuNormalShot : public Bullet
{
public:
	ReimuNormalShot(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>("images/Shot1.png"), std::make_unique<CircleCollider>(Vector2{ 0.0f, -23.0f }, 6.5f), EffectManager::EffectID::None, 4)
	{
		clip = { 2, 3, 13, 63 };
	}
};

/******************************** Player *********************************/

/// <param name="position">初期位置</param>
/// <param name="highSpeed">高速移動時の速さ（単位：ドット毎フレーム）</param>
/// <param name="lowSpeed">低速移動時の速さ（単位：ドット毎フレーム）</param>
/// <param name="sprite">Spriteクラスへのポインタ（画像はAssetLoaderから指定）</param>
/// <param name="collider">当たり判定クラスへのポインタ</param>
/// <param name="effectID">消滅エフェクトのID</param>
Player::Player(const Vector2& position, const float highSpeed, const float lowSpeed, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID)
	: Mover(position, 0.0f, -M_PI_2, std::move(sprite), std::move(collider), effectID, 1)
	, highSpeed(highSpeed)
	, lowSpeed(lowSpeed)
{
	for (int i = 0; i < static_cast<int>(clips.size()); i++)
		for (int j = 0; j < static_cast<int>(clips[i].size()); j++)
			clips[i][j] = { j * Width, i * Height, Width, Height };
}

void Player::Update()
{
	Mover::Update();

	// 移動制限。
	if ((position.x - Width * 0.5f < 0) || (position.x + Width * 0.5f > Game::Width))
		position.x -= velocity.x;
	if ((position.y - Width * 0.5f < 0) || (position.y + Height * 0.5f > Game::Height))
		position.y -= velocity.y;

	// 描画の前処理。
	if (sprite->GetAlpha() < 255) {
		if (Time->GetCountedFrames() - beginningFrame >= InvincibleFrames)
			sprite->SetAlpha(255);
		else if ((Time->GetCountedFrames() - beginningFrame) / 3 % 2 == 0)  // 3フレーム毎に点滅する。
			sprite->SetAlpha(0);
		else
			sprite->SetAlpha(191);
	}
}

void Player::OnCollide(Mover& mover)
{
	if (Time->GetCountedFrames() - beginningFrame < InvincibleFrames)
		return;
	enabled = false;
	--life;
}

void Player::Spawned()
{
	if (life <= 0)
		return;
	Mover::spawned();
	beginningFrame = Time->GetCountedFrames();
	sprite->SetAlpha(191);
}

void Player::Shoot()
{
	const int shotDelayFrames = 6;
	const float bulletSpeed = 30.0f;

	static int previousShootingFrame = 0;
	int currentFrame = Time->GetCountedFrames();
	if (currentFrame - previousShootingFrame > shotDelayFrames) {
		auto newLeftBullet = manager.lock()->GenerateObject(PlayerManager::BulletID::ReimuNormal, position - Vector2{ 12.0f, 0.0f });
		newLeftBullet.lock()->Shot(bulletSpeed, -M_PI_2);
		auto newRightBullet = manager.lock()->GenerateObject(PlayerManager::BulletID::ReimuNormal, position + Vector2{ 12.0f, 0.0f });
		newRightBullet.lock()->Shot(bulletSpeed, -M_PI_2);
		previousShootingFrame = currentFrame;
	}
}

SDL_Rect& Player::clipFromImage(unsigned int countedFrames)
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
};

/******************************** PlayerManager *********************************/

std::weak_ptr<Player> PlayerManager::GenerateObject(const PlayerID id, const Vector2& position)
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
	newObject.lock()->setManager(shared_from_this());
	return newObject;
}

std::weak_ptr<Bullet> PlayerManager::GenerateObject(const BulletID id, const Vector2& position)
{
	std::weak_ptr<Bullet> newObject;
	switch (id) {
	case BulletID::ReimuNormal:
		newObject = assignObject<ReimuNormalShot>(position);
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
