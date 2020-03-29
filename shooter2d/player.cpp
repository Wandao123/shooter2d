#include "bullet.h"
#include "media.h"
#include "player.h"

using namespace Shooter;

// 共有テクスチャ。PlayerManagerで初期化する。
static std::weak_ptr<SDL_Texture> ReimuImage;
static std::weak_ptr<SDL_Texture> MarisaImage;
static std::weak_ptr<SDL_Texture> SanaeImage;

/******************************** 個別設定用クラス *********************************/

/// <summary>自機キャラクタクラス。</summary>
class PlayerCharacter : public Player
{
public:
	static const int Height = 48;
	static const int Width = 32;

	/// <param name="position">初期位置</param>
	/// <param name="highSpeed">高速移動時の速さ（単位：ドット毎フレーム）</param>
	/// <param name="lowSpeed">低速移動時の速さ（単位：ドット毎フレーム）</param>
	/// <param name="sprite">Spriteクラスへのポインタ（画像はAssetLoaderから指定）</param>
	/// <param name="collider">当たり判定クラスへのポインタ</param>
	PlayerCharacter(const Vector2<double>& position, const double highSpeed, const double lowSpeed, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider)
		: Player(position, std::move(sprite), std::move(collider))
		, highSpeed(highSpeed)
		, lowSpeed(lowSpeed)
	{
		for (int i = 0; i < static_cast<int>(clips.size()); i++)
			for (int j = 0; j < static_cast<int>(clips[i].size()); j++)
				clips[i][j] = { j * Width, i * Height, Width, Height };
		this->sprite->SetClip(clips[0][0]);  // 最初のフレームだけ画像全体が表示されてしまうため、ここで明示的に代入。
	}

	void Update() override
	{
		Player::Update();

		// 移動制限。ただし、速度を変化させた場合のみで、位置を直接変える場合は制限しない。
		if (position.x - Width * 0.5 < 0 || position.x + Width * 0.5 > Media::Create().GetWidth())
			position.x -= GetVelocity().x;
		if (position.y - Height * 0.5 < 0 || position.y + Height * 0.5 > Media::Create().GetHeight())
			position.y -= GetVelocity().y;
		/* 復帰処理との兼ね合い（画面外から移動させる）から、次のようには書かない。
		if (position.x - Width * 0.5 < 0)
			position.x = Width * 0.5;
		else if (position.x + Width * 0.5 > Media::Create().GetWidth())
			position.x = Media::Create().GetWidth() - Width * 0.5;
		if (position.y - Height * 0.5 < 0)
			position.y = Height * 0.5;
		else if (position.y + Height * 0.5 > Media::Create().GetHeight())
			position.y = Media::Create().GetHeight() - Height * 0.5;*/
	}

	/// <summary>速度の方向を設定する。</summary>
	/// <param name="velocity">xy座標系での速度の方向</param>
	/// <remarks>速度の方向のみ考慮することに注意（大きさは規格化される）。速さは予め設定されたhighSpeed/lowSpeedを使う。</remarks>
	void SetVelocity(const Vector2<double>& velocity) override
	{
		Player::SetVelocity(velocity.Normalize() * (SlowMode ? lowSpeed : highSpeed));
	}
protected:
	Rect<int>& clipFromImage(unsigned int countedFrames) override
	{
		const int DelayFrames = 3;  // 左右移動の際に次の画像に切り替わるまでのフレーム数。
		const int NumSlice = 5;     // 停止時、左移動、右移動における各変化のコマ数。
		const int Period = 6;       // 停止時などに画像を繰り返す周期。
		static int level = 0;       // 左右に何フレーム進んでいるか表すフラグ。-(DelayFrames * NumSlice - 1) .. DelayFrames * NumSlice - 1 の範囲を動く。
		if (GetVelocity().x < 0.0)
			level = std::max(level - 1, -(DelayFrames * NumSlice - 1));
		else if (GetVelocity().x > 0.0)
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
private:
	std::array<std::array<Rect<int>, 5>, 3> clips;  // 3成分はそれぞれ停止時、左移動、右移動を表す。5成分は変化の差分を表す。
	const double highSpeed;
	const double lowSpeed;
};

/// <summary>自機オプションクラス。</summary>
/// <remarks>現在の実装では当たり判定は無い。</remarks>
class PlayerOption : public Player
{
public:
	PlayerOption(const Vector2<double>& position, std::unique_ptr<Sprite>&& sprite)
		: Player(position, std::move(sprite), nullptr)
		, clip({ 0, 144, 16, 16 })
	{
		this->sprite->SetClip(clip);  // 最初のフレームだけ画像全体が表示されてしまうため、ここで明示的に代入。
	}
protected:
	Rect<int>& clipFromImage(unsigned int) override
	{
		return clip;
	}
private:
	Rect<int> clip;
};

class Reimu : public PlayerCharacter
{
public:
	Reimu(const Vector2<double>& position)
		: PlayerCharacter(position, 4.5, 2.0, std::make_unique<Sprite>(ReimuImage), std::make_unique<CircleCollider>(1.0))
	{}
};

class Marisa : public PlayerCharacter
{
public:
	Marisa(const Vector2<double>& position)
		: PlayerCharacter(position, 5.0, 2.0, std::make_unique<Sprite>(MarisaImage), std::make_unique<CircleCollider>(1.3))
	{}
};

class Sanae : public PlayerCharacter
{
public:
	Sanae(const Vector2<double>& position)
		: PlayerCharacter(position, 4.5, 2.0, std::make_unique<Sprite>(SanaeImage), std::make_unique<CircleCollider>(1.3))
	{}
};

class ReimuOption : public PlayerOption
{
public:
	ReimuOption(const Vector2<double>& position)
		: PlayerOption(position, std::make_unique<Sprite>(ReimuImage))
	{}

	void Update() override
	{
		rotationAngle += M_PI / 60;
	}

	void Draw() const override
	{
		sprite->Draw(position, rotationAngle, 1.0);
	}
private:
	double rotationAngle = 0.e0;
};

class MarisaOption : public PlayerOption
{
public:
	MarisaOption(const Vector2<double>& position)
		: PlayerOption(position, std::make_unique<Sprite>(MarisaImage))
	{}

	void Update() override
	{
		rotationAngle += M_PI / 60;
	}

	void Draw() const override
	{
		sprite->Draw(position, rotationAngle, 1.0);
	}
private:
	double rotationAngle = 0.e0;
};

class SanaeOption : public PlayerOption
{
public:
	SanaeOption(const Vector2<double>& position)
		: PlayerOption(position, std::make_unique<Sprite>(SanaeImage))
	{}

	void Update() override
	{
		rotationAngle += M_PI / 60;
	}

	void Draw() const override
	{
		sprite->Draw(position, rotationAngle, 1.0);
	}
private:
	double rotationAngle = 0.e0;
};

/******************************** Player *********************************/

/// <param name="position">初期位置</param>
/// <param name="sprite">Spriteクラスへのポインタ（画像はAssetLoaderから指定）</param>
/// <param name="collider">当たり判定クラスへのポインタ</param>
Player::Player(const Vector2<double>& position, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider)
	: Mover(position, 0.0, -M_PI_2, std::move(sprite), std::move(collider), 1, 3, false)
{}

void Player::Update()
{
	Mover::Update();

	// 描画の前処理。
	if (sprite->GetAlpha() < 0xFF) {
		if (invincibleCounter == 0)
			sprite->SetAlpha(0xFF);
		else if (invincibleCounter / 3 % 2 == 0)  // 3フレーム毎に点滅する。
			sprite->SetAlpha(0);
		else
			sprite->SetAlpha(0xBF);  // 0xFF * 3 / 4
	}
}

void Player::OnCollide(Mover&)
{
	if (invincibleCounter > 0)
		return;
	enabled = false;
	--hitPoint;
}

void Player::Spawned()
{
	if (hitPoint <= 0)
		return;
	Mover::spawned();
	SetSpeed(0.0);
	SetAngle(-M_PI_2);
	velocity = { 0.0, 0.0 };
	sprite->SetAlpha(0xBF);
}

/******************************** PlayerManager *********************************/

PlayerManager::PlayerManager()
	: CharacterSize({ PlayerCharacter::Width, PlayerCharacter::Height })
{
	ReimuImage = AssetLoader::Create().GetTexture("images/Reimudot.png");
	MarisaImage = AssetLoader::Create().GetTexture("images/Marisadot.png");
	SanaeImage = AssetLoader::Create().GetTexture("images/Sanaedot.png");
}

std::weak_ptr<Player> PlayerManager::GenerateObject(const PlayerID id, const Vector2<double>& position)
{
	std::weak_ptr<Player> newObject;
	switch (id) {
	// 自機。
	case PlayerID::Reimu:
		newObject = assignObject<Reimu>(position);
		break;
	case PlayerID::Marisa:
		newObject = assignObject<Marisa>(position);
		break;
	case PlayerID::Sanae:
		newObject = assignObject<Sanae>(position);
		break;
	// オプション。
	case PlayerID::ReimuOption:
		newObject = assignObject<ReimuOption>(position);
		break;
	case PlayerID::MarisaOption:
		newObject = assignObject<MarisaOption>(position);
		break;
	case PlayerID::SanaeOption:
		newObject = assignObject<SanaeOption>(position);
		break;
	}
	return newObject;
}

std::weak_ptr<Player> PlayerManager::GetPlayer() const
{
	for (auto&& object : objectsList) {
		auto temp = std::dynamic_pointer_cast<PlayerCharacter>(object);
		if (temp.get() != nullptr)  // インスタンスの親子関係を調べる。もっとよい方法はないのか？
			return temp;
	}
	return {};
}
