#include <algorithm>
#include "game.h"
#include "mover.h"

using namespace Shooter;

std::unique_ptr<AssetLoader> assetLoader = std::make_unique<AssetLoader>();

class Reimu : public Player
{
public:
	Reimu(const Vector2& position)
		: Player(position, 4.5f, 2.0f, std::make_shared<Sprite>(assetLoader->GetTexture("images/Reimudot.png")), std::make_unique<CircleCollider>(position, 1.0f))
	{}
};

class Marisa : public Player
{
public:
	Marisa(const Vector2& position)
		: Player(position, 5.0f, 2.0f, std::make_shared<Sprite>(assetLoader->GetTexture("images/Marisadot.png")), std::make_unique<CircleCollider>(position, 1.3f))
	{}
};

class Sanae : public Player
{
public:
	Sanae(const Vector2& position)
		: Player(position, 4.5f, 2.0f, std::make_shared<Sprite>(assetLoader->GetTexture("images/Sanaedot.png")), std::make_unique<CircleCollider>(position, 1.3f))
	{}
};

class SmallBlueEnemy : public Enemy
{
public:
	SmallBlueEnemy(const Vector2& position)
		: Enemy(position, std::make_shared<Shooter::Sprite>(assetLoader->GetTexture("images/Enemy.png")), std::make_unique<CircleCollider>(position, Enemy::Width * 0.5f))
	{
		InitializeClips();
	}
protected:
	void InitializeClips() override
	{
		for (int j = 0; j < clips[0].size(); j++) {
			clips[0][j] = { j * Width, 0, Width, Height };                 // 停止時
			clips[1][j] = { (j + 3) * Width, 2 * Height, Width, Height };  // 左移動
			clips[2][j] = { (j + 3) * Width, 0, Width, Height };           // 右移動
		}
	}
};

void Mover::Draw()
{
	sprite->Draw(static_cast<int>(position.x - sprite->GetClip()->w * 0.5f),
		static_cast<int>(position.y - sprite->GetClip()->h * 0.5f));
}

void Mover::Spawned(const float speed, const float angle)
{
	enabled = true;
	counter = 0;
	SetSpeed(speed);
	SetAngle(angle);
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
	collider->SetPosition(position);
}

bool Mover::isInside()
{
	if (counter < 60) {
		++counter;
		return true;
	}
	if (position.x < -sprite->GetClip()->w / 2 || position.x > Game::Width + sprite->GetClip()->w / 2
		|| position.y < -sprite->GetClip()->h / 2 || position.y > Game::Height + sprite->GetClip()->h / 2)
		return false;
	else
		return true;
}

Player::Player(const Vector2& position, const float highSpeed, const float lowSpeed,
	const std::shared_ptr<Sprite> sprite, std::unique_ptr<Collider>&& collider)
	: Mover(position, highSpeed, -M_PI_2, sprite, std::move(collider))
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
	sprite->SetClip(std::make_shared<SDL_Rect>(currentClip));
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
	Mover::Update();
}

void Player::move()
{
	position += velocity * Time->GetDeltaTime();
	if ((position.x - Width * 0.5f < 0) || (position.x + Width * 0.5f > Game::Width))
		position.x -= velocity.x * Time->GetDeltaTime();
	if ((position.y - Width * 0.5f < 0) || (position.y + Height * 0.5f > Game::Height))
		position.y -= velocity.y * Time->GetDeltaTime();
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
	return newObject;
}

Enemy::Enemy(const Vector2& position, const std::shared_ptr<Sprite> sprite, std::unique_ptr<Collider>&& collider)
	: Mover(position, 0.0f, M_PI_2, sprite, std::move(collider))
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
	sprite->SetClip(std::make_shared<SDL_Rect>(currentClip));
	Mover::Draw();
}

void Enemy::Update()
{
	// 1フレームを単位時間とする。
	position.x += speed * std::cos(angle);
	position.y += speed * std::sin(angle);
	Mover::Update();
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