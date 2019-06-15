﻿#include <algorithm>
#include "game.h"
#include "mover.h"

using namespace Shooter;

extern std::unique_ptr<AssetLoader> assetLoader;

class Reimu : public Player
{
public:
	Reimu(const Vector2& position)
		: Player(position, 4.5f, 2.0f, std::make_unique<Sprite>(assetLoader->GetTexture("images/Reimudot.png")), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 1.0f), EffectManager::EffectID::BlueCircle)
	{}
};

class Marisa : public Player
{
public:
	Marisa(const Vector2& position)
		: Player(position, 5.0f, 2.0f, std::make_unique<Sprite>(assetLoader->GetTexture("images/Marisadot.png")), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 1.3f), EffectManager::EffectID::BlueCircle)
	{}
};

class Sanae : public Player
{
public:
	Sanae(const Vector2& position)
		: Player(position, 4.5f, 2.0f, std::make_unique<Sprite>(assetLoader->GetTexture("images/Sanaedot.png")), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 1.3f), EffectManager::EffectID::BlueCircle)
	{}
};

class SmallBlueEnemy : public Enemy
{
public:
	SmallBlueEnemy(const Vector2& position)
		: Enemy(position, std::make_unique<Sprite>(assetLoader->GetTexture("images/Enemy.png")), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, Enemy::Width * 0.5f), EffectManager::EffectID::BlueCircle)
	{
		for (int j = 0; j < clips[0].size(); j++) {
			clips[0][j] = { j * Width, 0, Width, Height };                 // 停止時
			clips[1][j] = { (j + 3) * Width, 2 * Height, Width, Height };  // 左移動
			clips[2][j] = { (j + 3) * Width, 0, Width, Height };           // 右移動
		}
	}
};

class ReimuNormalShot : public Bullet
{
public:
	ReimuNormalShot(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>(assetLoader->GetTexture("images/Shot1.png"), std::make_unique<SDL_Rect>(SDL_Rect{ 2, 3, 13, 63 })), std::make_unique<CircleCollider>(Vector2{ 0.0f, -23.0f }, 6.5f), EffectManager::EffectID::None, 4)
	{}
};

// 実際の画像よりも判定は小さい。
class SmallBullet : public Bullet
{
public:
	SmallBullet(const Vector2& position)
		: Bullet(position, std::make_unique<Sprite>(assetLoader->GetTexture("images/shot_all.png"), std::make_unique<SDL_Rect>(SDL_Rect{ 1, 13, 15, 15 })), std::make_unique<CircleCollider>(Vector2{ 0.0f, 0.0f }, 7.0f), EffectManager::EffectID::BlueCircle, 1)
	{}
};

void Mover::Draw()
{
	sprite->Draw(position);
}

void Mover::Spawned()
{
	enabled = true;
	counter = 0;
}

void Mover::Update()
{
	if (!isInside())
		enabled = false;
}

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

Bullet::Bullet(const Vector2& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID, unsigned int damage)
	: Mover(position, 0.0f, M_PI_2, std::move(sprite), std::move(collider), effectID, damage)
{}

void Bullet::Draw()
{
	sprite->Draw(position, angle + M_PI_2, 1.0f);  // 元の画像は -PI/2 の向きが正位置。よって、画像の回転角は +PI/2 される。
}

void Bullet::Update()
{
	// 1フレームを単位時間とする。
	position.x += speed * std::cos(angle);
	position.y += speed * std::sin(angle);
	Mover::Update();
}

void Bullet::OnCollide(Mover& mover)
{
	enabled = false;
}

void Bullet::Shot(const float speed, const float angle)
{
	Mover::Spawned();
	SetSpeed(speed);
	SetAngle(angle);
}

std::shared_ptr<Bullet> BulletManager::GenerateObject(const BulletID id, const Vector2& position)
{
	std::shared_ptr<Bullet> newObject;
	switch (id) {
	case BulletID::Small:
		newObject = assignObject<SmallBullet>(position);
		break;
	}
	newObject->SetSpeed(0.0f);
	newObject->SetAngle(M_PI_2);
	return std::move(newObject);
}

Player::Player(const Vector2& position, const float highSpeed, const float lowSpeed, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID)
	: Mover(position, highSpeed, -M_PI_2, std::move(sprite), std::move(collider), effectID, 1)
	, lowSpeed(lowSpeed)
	, velocity({ 0.0f, 0.0f })
{
	for (int i = 0; i < clips.size(); i++)
		for (int j = 0; j < clips[i].size(); j++)
			clips[i][j] = { j * Width, i * Height, Width, Height };
}

void Player::Draw()
{
	// HACK: ここはファイル構造に強く依存するため、アルゴリズムだけ巧く抽出できないか？
	auto clipFromImage = [this](Uint32 countedFrames) -> SDL_Rect& {
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

	SDL_Rect& currentClip = clipFromImage(Time->GetCountedFrames());
	sprite->SetClip(currentClip);
	Mover::Draw();
}

void Player::Update()
{
	// TODO: キー入力処理の分離。
	float speedPerSecond;  // 単位：ドット毎秒
	const SDL_Keymod modStates = SDL_GetModState();

	if (modStates & KMOD_SHIFT)
		speedPerSecond = lowSpeed * Timer::FPS;
	else
		speedPerSecond = speed * Timer::FPS;
	velocity = { 0.0f, 0.0f };
	const Uint8 *currentKeyStates = SDL_GetKeyboardState(nullptr);
	if (currentKeyStates[SDL_SCANCODE_LEFT])
		velocity.x = -speedPerSecond;
	if (currentKeyStates[SDL_SCANCODE_RIGHT])
		velocity.x = +speedPerSecond;
	if (currentKeyStates[SDL_SCANCODE_UP])
		velocity.y = -speedPerSecond;
	if (currentKeyStates[SDL_SCANCODE_DOWN])
		velocity.y = +speedPerSecond;
	velocity = velocity.Normalize() * speedPerSecond;
	move();

	if (currentKeyStates[SDL_SCANCODE_Z])
		shoot();

	Mover::Update();
}

void Player::OnCollide(Mover& mover)
{
	enabled = false;
}

void Player::move()
{
	position += velocity * Time->GetDeltaTime();
	if ((position.x - Width * 0.5f < 0) || (position.x + Width * 0.5f > Game::Width))
		position.x -= velocity.x * Time->GetDeltaTime();
	if ((position.y - Width * 0.5f < 0) || (position.y + Height * 0.5f > Game::Height))
		position.y -= velocity.y * Time->GetDeltaTime();
}

void Player::shoot()
{
	const int shotDelayFrames = 6;
	const float bulletSpeed = 30.0f;

	static int previousShootingFrame = 0;
	int currentFrame = Time->GetCountedFrames();
	if (currentFrame - previousShootingFrame > shotDelayFrames) {
		auto newLeftBullet = manager.lock()->GenerateObject(PlayerManager::BulletID::ReimuNormal, position - Vector2{ 12.0f, 0.0f });
		auto newRightBullet = manager.lock()->GenerateObject(PlayerManager::BulletID::ReimuNormal, position + Vector2{ 12.0f, 0.0f });
		newLeftBullet->Shot(bulletSpeed, -M_PI_2);
		newRightBullet->Shot(bulletSpeed, -M_PI_2);
		previousShootingFrame = currentFrame;
	}
}

std::shared_ptr<Player> PlayerManager::GenerateObject(const PlayerID id, const Vector2& position)
{
	std::shared_ptr<Player> newObject;
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
	newObject->SetManager(shared_from_this());
	return newObject;
}

std::shared_ptr<Bullet> PlayerManager::GenerateObject(const BulletID id, const Vector2& position)
{
	std::shared_ptr<Bullet> newObject;
	switch (id) {
	case BulletID::ReimuNormal:
		newObject = assignObject<ReimuNormalShot>(position);
		break;
	}
	newObject->SetSpeed(0.0f);
	newObject->SetAngle(M_PI_2);
	return std::move(newObject);
}

std::shared_ptr<Player> PlayerManager::GetPlayer() const
{
	for (auto&& object : objectsList) {
		auto temp = std::dynamic_pointer_cast<Player>(object);
		if (temp.get() != nullptr)  // インスタンスの親子関係を調べる。もっとよい方法はないのか？
			return temp;
	}
	return nullptr;
}

Enemy::Enemy(const Vector2& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID)
	: Mover(position, 0.0f, M_PI_2, std::move(sprite), std::move(collider), effectID, 1)
{}

void Enemy::Draw()
{
	// 30度ずつの領域に分けて、画像を切り換える。
	auto clipFromImage = [this](Uint32 countedFrames) -> SDL_Rect& {
		const int DelayFrames = 6;
		const int NumSlice = 3;
		if (speed > 0.0f) {
			if (angle == M_PI * 3.0f / 2.0f)
				return clips[0][(countedFrames / DelayFrames) % NumSlice];  // どう設定するのが適切なのか？
			else if (angle > M_PI * 11.0f / 12.0f && angle < M_PI * 3.0f / 2.0f)
				return clips[1][2];
			else if (angle > M_PI * 3.0f / 4.0f && angle <= M_PI * 11.0f / 12.0f)
				return clips[1][1];
			else if (angle > M_PI * 7.0f / 12.0f && angle <= M_PI * 3.0f / 4.0f)
				return clips[1][0];
			else if (angle >= M_PI * 5.0f / 12.0f && angle <= M_PI * 7.0f / 12.0f)
				return clips[0][(countedFrames / DelayFrames) % NumSlice];
			else if (angle >= M_PI / 4.0f && angle < M_PI * 5.0f / 12.0f)
				return clips[2][0];
			else if (angle >= M_PI / 12.0f && angle < M_PI / 4.0f)
				return clips[2][1];
			else
				return clips[2][2];
		} else {
			return clips[0][(countedFrames / DelayFrames) % NumSlice];
		}
	};

	SDL_Rect& currentClip = clipFromImage(Time->GetCountedFrames());
	sprite->SetClip(currentClip);
	if (isDamaged) {
		sprite->SetColor(128, 128, 128);
		isDamaged = false;
	} else {
		sprite->SetColor(255, 255, 255);
	}
	Mover::Draw();
}

void Enemy::Update()
{
	// 1フレームを単位時間とする。
	position.x += speed * std::cos(angle);
	position.y += speed * std::sin(angle);
	Mover::Update();
}

void Enemy::OnCollide(Mover& mover)
{
	hitPoint -= mover.GetDamage();
	if (hitPoint <= 0)
		enabled = false;
	else
		isDamaged = true;
}

void Enemy::Spawned(const float speed, const float angle, const int hitPoint)
{
	Mover::Spawned();
	SetSpeed(speed);
	SetAngle(angle);
	this->hitPoint = hitPoint;
}

std::shared_ptr<Enemy> EnemyManager::GenerateObject(const EnemyID id, const Vector2& position)
{
	std::shared_ptr<Enemy> newObject;
	switch (id) {
	case EnemyID::SmallBlue:
		newObject = assignObject<SmallBlueEnemy>(position);
		break;
	}
	newObject->SetSpeed(0.0f);
	newObject->SetAngle(M_PI_2);
	return std::move(newObject);
}