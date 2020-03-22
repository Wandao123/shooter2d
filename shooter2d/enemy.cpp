#include "enemy.h"

using namespace Shooter;

/******************************** 個別設定用クラス *********************************/

class SmallRedEnemy : public Enemy
{
public:
	SmallRedEnemy(const Vector2<double>& position)
		: Enemy(position, std::make_unique<Sprite>(AssetLoader::Create().GetTexture("images/Enemy.png")), std::make_unique<RectangleCollider>(Vector2<double>(), M_PI_4, Vector2<double>{ Enemy::Width, Enemy::Height }))
		//std::make_unique<CircleCollider>(Enemy::Width * 0.5)
	{
		for (int j = 0; j < static_cast<int>(clips[0].size()); j++) {
			clips[0][j] = { j * Width, Height, Width, Height };            // 停止時
			clips[1][j] = { (j + 3) * Width, 3 * Height, Width, Height };  // 左移動
			clips[2][j] = { (j + 3) * Width, Height, Width, Height };      // 右移動
		}
	}
};

class SmallBlueEnemy : public Enemy
{
public:
	SmallBlueEnemy(const Vector2<double>& position)
		: Enemy(position, std::make_unique<Sprite>(AssetLoader::Create().GetTexture("images/Enemy.png")), std::make_unique<CircleCollider>(Enemy::Width * 0.5))
	{
		for (int j = 0; j < static_cast<int>(clips[0].size()); j++) {
			clips[0][j] = { j * Width, 0, Width, Height };                 // 停止時
			clips[1][j] = { (j + 3) * Width, 2 * Height, Width, Height };  // 左移動
			clips[2][j] = { (j + 3) * Width, 0, Width, Height };           // 右移動
		}
	}
};

/******************************** Enemy *********************************/

/// <param name="position">初期位置</param>
/// <param name="sprite">Spriteクラスへのポインタ（画像はAssetLoaderから指定）</param>
/// <param name="collider">当たり判定クラスへのポインタ</param>
Enemy::Enemy(const Vector2<double>& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider)
	: Mover(position, 0.0, M_PI_2, std::move(sprite), std::move(collider), 1, 0)
	, sound(std::make_unique<Sound>(AssetLoader::Create().GetChunk("se/enemy_damage.wav")))
{
	this->sound->SetVolume(Sound::MaxVolume / 4);
}

void Enemy::Update()
{
	Mover::Update();
	if (isDamaged) {
		sprite->SetColor(128, 128, 128);
		isDamaged = false;
	}
	else {
		sprite->SetColor(255, 255, 255);
	}
}

void Enemy::OnCollide(Mover& mover)
{
	hitPoint -= mover.GetDamage();
	if (hitPoint <= 0)
		enabled = false;
	else
		isDamaged = true;

	sound->Played();
}

void Enemy::Spawned(const double speed, const double angle, const int hitPoint)
{
	Mover::spawned();
	SetSpeed(speed);
	SetAngle(angle);
	this->hitPoint = hitPoint;
}

Rect<int>& Enemy::clipFromImage(unsigned int countedFrames)
{
	const int DelayFrames = 6;
	const int NumSlice = 3;
	if (speed > 0.0) {  // 30度ずつの領域に分けて、画像を切り換える。
		if (angle == M_PI * 3.0 / 2.0)
			return clips[0][(countedFrames / DelayFrames) % NumSlice];  // どう設定するのが適切なのか？
		else if (angle > M_PI * 11.0 / 12.0 && angle < M_PI * 3.0 / 2.0)
			return clips[1][2];
		else if (angle > M_PI * 3.0 / 4.0 && angle <= M_PI * 11.0 / 12.0)
			return clips[1][1];
		else if (angle > M_PI * 7.0 / 12.0 && angle <= M_PI * 3.0 / 4.0)
			return clips[1][0];
		else if (angle >= M_PI * 5.0 / 12.0 && angle <= M_PI * 7.0 / 12.0)
			return clips[0][(countedFrames / DelayFrames) % NumSlice];
		else if (angle >= M_PI / 4.0 && angle < M_PI * 5.0 / 12.0)
			return clips[2][0];
		else if (angle >= M_PI / 12.0 && angle < M_PI / 4.0)
			return clips[2][1];
		else
			return clips[2][2];
	}
	else {
		return clips[0][(countedFrames / DelayFrames) % NumSlice];
	}
}

/******************************** EnemyManager *********************************/

std::weak_ptr<Enemy> EnemyManager::GenerateObject(const EnemyID id, const Vector2<double>& position)
{
	std::weak_ptr<Enemy> newObject;
	switch (id) {
	case EnemyID::SmallRed:
		newObject = assignObject<SmallRedEnemy>(position);
		break;
	case EnemyID::SmallBlue:
		newObject = assignObject<SmallBlueEnemy>(position);
		break;
	}
	return newObject;
}
