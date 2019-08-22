#include "game.h"

using namespace Shooter;

/******************************** 非公開クラス *********************************/

class GameOverScene : public Scene
{
public:
	GameOverScene(IChangingSceneListener& listener);
	void Draw() const override;
	void Update() override;
private:
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	unsigned int createdFrame;
};

class GameClearScene : public Scene
{
public:
	GameClearScene(IChangingSceneListener& listener);
	void Draw() const override;
	void Update() override;
private:
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	unsigned int createdFrame;
};

class GameAllClearScene : public Scene
{
public:
	GameAllClearScene(IChangingSceneListener& listener);
	void Draw() const override;
	void Update() override;
private:
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	unsigned int createdFrame;
};

class GameScene : public Scene
{
public:
	GameScene(IChangingSceneListener& listener);
	void Draw() const override;
	void Update() override;
private:
	std::shared_ptr<BulletManager> bulletManager;
	std::shared_ptr<EffectManager> effectManager;
	std::shared_ptr<EnemyManager> enemyManager;
	std::shared_ptr<PlayerManager> playerManager;
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	std::unique_ptr<CollisionDetector> collisionDetector;
	std::unique_ptr<Script> script;
};

class KeyConfigScene : public Scene
{
public:
	KeyConfigScene(IChangingSceneListener& listener);
	void Draw() const override;
	void Update() override;
private:
	static const int MaxItems = 10;
	const int ItemHeight = 35;  // フォントサイズを考慮。
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	int currentIndex;
	std::array<std::weak_ptr<UserInterface>, MaxItems> menu;
};

/******************************** TitleScene *********************************/

TitleScene::TitleScene(IChangingSceneListener& listener)
	: Scene(listener)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, currentIndex(0)
{
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2{ Game::Width * 0.5f, Game::Height * 1.0f / 5.0f }).lock()->SetCaption(u8"Bullet Hell 2D Shmup");
	menu[0] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, Vector2{ Game::Width * 0.5f, Game::Height * 2.0f / 5.0f });
	menu[0].lock()->SetCaption(u8"Start");
	menu[1] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, Vector2{ Game::Width * 0.5f, Game::Height * 2.0f / 5.0f + ItemHeight });
	menu[1].lock()->SetCaption(u8"Key config");
	menu[2] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, Vector2{ Game::Width * 0.5f, Game::Height * 2.0f / 5.0f + ItemHeight * 2 });
	menu[2].lock()->SetCaption(u8"Quit");
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2{ Game::Width - 56, Game::Height - 7 });
}

void TitleScene::Draw() const
{
	userInterfaceManager->Draw();
}

void TitleScene::Update()
{
	// 項目の選択。
	const unsigned int DelayFrames = 12;
	static unsigned int previousPressedFrame = Timer::Create().GetCountedFrames();
	if (Timer::Create().GetCountedFrames() - previousPressedFrame >= DelayFrames) {
		if (Input::Create().GetKey(Input::Commands::Up)) {
			currentIndex = MathUtils::Modulo(currentIndex - 1, MaxItems);
			previousPressedFrame = Timer::Create().GetCountedFrames();
		}
		if (Input::Create().GetKey(Input::Commands::Down)) {
			currentIndex = MathUtils::Modulo(currentIndex + 1, MaxItems);
			previousPressedFrame = Timer::Create().GetCountedFrames();
		}
	} else {
		if (Input::Create().GetKeyDown(Input::Commands::Up)) {
			currentIndex = MathUtils::Modulo(currentIndex - 1, MaxItems);
			previousPressedFrame = Timer::Create().GetCountedFrames();
		}
		if (Input::Create().GetKeyDown(Input::Commands::Down)) {
			currentIndex = MathUtils::Modulo(currentIndex + 1, MaxItems);
			previousPressedFrame = Timer::Create().GetCountedFrames();
		}
	}

	// 現在の選択肢に対する処理。
	for (int i = 0; i < MaxItems; i++)
		menu[i].lock()->SetActive((i != currentIndex) ? false : true);
	userInterfaceManager->Update();  // ClearAndChangeSceneを実行してから更新するとエラー。
	if (Input::Create().GetKeyDown(Input::Commands::OK)) {
		switch (currentIndex) {
		case 0:
			listener.ClearAndChangeScene(std::make_unique<GameScene>(listener));
			break;
		case 1:
			listener.PushScene(std::make_unique<KeyConfigScene>(listener));
			break;
		case 2:
			listener.Quit();
			break;
		}
	}
}

/******************************** GameOverScene *********************************/

GameOverScene::GameOverScene(IChangingSceneListener& listener)
	: Scene(listener)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, createdFrame(Timer::Create().GetCountedFrames())
{
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2{ Game::Width * 0.5f, Game::Height * 0.5f }).lock()->SetCaption("GAME OVER");
}

void GameOverScene::Draw() const
{
	userInterfaceManager->Draw();
}

void GameOverScene::Update()
{
	userInterfaceManager->Update();
	if (Timer::Create().GetCountedFrames() - createdFrame > Timer::FPS * 3)
		listener.ClearAndChangeScene(std::make_unique<TitleScene>(listener));
}

/******************************** GameClearScene *********************************/

GameClearScene::GameClearScene(IChangingSceneListener& listener)
	: Scene(listener)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, createdFrame(Timer::Create().GetCountedFrames())
{
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2{ Game::Width * 0.5f, Game::Height * 0.5f }).lock()->SetCaption("STAGE CLEAR");
}

void GameClearScene::Draw() const
{
	userInterfaceManager->Draw();
}

void GameClearScene::Update()
{
	userInterfaceManager->Update();
	if (Timer::Create().GetCountedFrames() - createdFrame > Timer::Create().FPS * 3)
		listener.PopScene();
}

/******************************** GameAllClearScene *********************************/

GameAllClearScene::GameAllClearScene(IChangingSceneListener& listener)
	: Scene(listener)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, createdFrame(Timer::Create().GetCountedFrames())
{
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2{ Game::Width * 0.5f, Game::Height * 0.5f }).lock()->SetCaption("ALL CLEAR");
}

void GameAllClearScene::Draw() const
{
	userInterfaceManager->Draw();
}

void GameAllClearScene::Update()
{
	userInterfaceManager->Update();
	if (Timer::Create().GetCountedFrames() - createdFrame > Timer::Create().FPS * 3)
		listener.ClearAndChangeScene(std::make_unique<TitleScene>(listener));
}

/******************************** GameScene *********************************/

GameScene::GameScene(IChangingSceneListener& listener)
	: Scene(listener)
	, bulletManager(std::make_shared<BulletManager>())
	, effectManager(std::make_shared<EffectManager>())
	, enemyManager(std::make_shared<EnemyManager>())
	, playerManager(std::make_shared<PlayerManager>())
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, collisionDetector(std::make_unique<CollisionDetector>(*bulletManager, *effectManager, *enemyManager, *playerManager))
	, script(std::make_unique<Script>(*bulletManager, *enemyManager, *playerManager))
{
	// 更新対象オブジェクトを生成。
	playerManager->GenerateObject(PlayerManager::PlayerID::Reimu, Vector2{ Game::Width / 2.0f, Game::Height - Player::Height }).lock()->Spawned();
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2{ Game::Width - 56, Game::Height - 7 });

	// 生成に時間が掛かるため、タイマーも初期化。
	Timer::Create().Start();
}

void GameScene::Update()
{
	auto updatePlayer = [this](Player& player) {
		// 自機の復活処理。
		const unsigned int DelayFrames = 30;
		static unsigned int playerDefeatedFrame = Timer::Create().GetCountedFrames();
		if (!player.IsEnabled() && player.GetLife() > 0 && Timer::Create().GetCountedFrames() - playerDefeatedFrame >= DelayFrames) {
			player.SetPosition(Vector2{ Game::Width / 2.0f, Game::Height - Player::Height });
			player.Spawned();
			playerDefeatedFrame = Timer::Create().GetCountedFrames();
		}
		
		// 自機の移動。
		float speed = Input::Create().GetKey(Input::Commands::Slow) ? player.GetLowSpeed() : player.GetHighSpeed();
		Vector2 velocity = { 0.0f, 0.0f };
		if (Input::Create().GetKey(Input::Commands::Leftward))
			velocity.x = -speed;
		if (Input::Create().GetKey(Input::Commands::Rightward))
			velocity.x = +speed;
		if (Input::Create().GetKey(Input::Commands::Forward))
			velocity.y = -speed;
		if (Input::Create().GetKey(Input::Commands::Backward))
			velocity.y = +speed;
		velocity = velocity.Normalize() * speed;
		player.SetVelocity(velocity);

		// 自機のショット。
		if (Input::Create().GetKey(Input::Commands::Shot)) {
			const unsigned int ShotDelayFrames = 6;
			static unsigned int playerShootingFrame = Timer::Create().GetCountedFrames();
			if (Timer::Create().GetCountedFrames() - playerShootingFrame >= ShotDelayFrames) {
				player.Shoot();
				playerShootingFrame = Timer::Create().GetCountedFrames();
			}
		}
	};

	auto player = playerManager->GetPlayer().lock();
	updatePlayer(*player);
	auto status = script->Run();
	bulletManager->Update();
	effectManager->Update();
	enemyManager->Update();
	playerManager->Update();
	userInterfaceManager->Update();
	collisionDetector->CheckAll();
	
	if (player->GetLife() <= 0) {
		static unsigned int counter = 0;
		if (counter >= 30)
			listener.PushScene(std::make_unique<GameOverScene>(listener));
		else
			++counter;
	}
	switch (status) {
	case Script::Status::AllClear:
		listener.PushScene(std::make_unique<GameAllClearScene>(listener));
		break;
	case Script::Status::Dead:
		listener.ClearAndChangeScene(std::make_unique<TitleScene>(listener));
		break;
	case Script::Status::Running:
		break;
	case Script::Status::StageClear:
		player->Erase();
		listener.PushScene(std::make_unique<GameClearScene>(listener));
		break;
	}
}

// SDLにはZ-orderの概念が無いため、描画のタイミングで順番に注意する必要がある。
void GameScene::Draw() const
{
	userInterfaceManager->Draw();
	effectManager->Draw();
	playerManager->Draw();
	enemyManager->Draw();
	bulletManager->Draw();
}

/******************************** KeyConfigScene *********************************/

KeyConfigScene::KeyConfigScene(IChangingSceneListener& listener)
	: Scene(listener)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, currentIndex(0)
{
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2{ Game::Width * 0.5f, Game::Height * 1.0f / 10.0f }).lock()->SetCaption(u8"Key config");
	auto makePos = [this](unsigned int index) -> Vector2 { return { Game::Width * 0.5f, Game::Height * 2.0f / 10.0f + ItemHeight * index }; };
	menu[0] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::ShotKeyConfig, makePos(0));
	menu[0].lock()->SetCaption(u8"Shot");
	menu[1] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::BombKeyConfig, makePos(1));
	menu[1].lock()->SetCaption(u8"Bomb");
	menu[2] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::SlowKeyConfig, makePos(2));
	menu[2].lock()->SetCaption(u8"Slow");
	menu[3] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::SkipKeyConfig, makePos(3));
	menu[3].lock()->SetCaption(u8"Skip");
	menu[4] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::LeftwardKeyConfig, makePos(4));
	menu[4].lock()->SetCaption(u8"Leftward");
	menu[5] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::RightwardKeyConfig, makePos(5));
	menu[5].lock()->SetCaption(u8"Rightward");
	menu[6] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::ForwardKeyConfig, makePos(6));
	menu[6].lock()->SetCaption(u8"Forward");
	menu[7] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::BackwardKeyConfig, makePos(7));
	menu[7].lock()->SetCaption(u8"Backward");
	menu[8] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::PauseKeyConfig, makePos(8));
	menu[8].lock()->SetCaption(u8"Pause");
	menu[9] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(9));
	menu[9].lock()->SetCaption(u8"Quit");
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2{ Game::Width - 56, Game::Height - 7 });
}

namespace Shooter {
	extern SDL_Renderer* Renderer;
}

void KeyConfigScene::Draw() const
{
	userInterfaceManager->Draw();

	int posX = static_cast<int>(Game::Width * 0.5f), posY = static_cast<int>(Game::Height * 2.0f / 10.0f + ItemHeight * currentIndex);
	int width = 14 * 3 * 14, height = ItemHeight;
	SDL_Rect rect = { posX - width / 2, posY - height / 2, width, height };
	SDL_SetRenderDrawColor(Renderer, 0xFF, 0xFF, 0x00, 63);
	SDL_SetRenderDrawBlendMode(Renderer, SDL_BLENDMODE_BLEND);
	SDL_RenderFillRect(Renderer, &rect);
}

void KeyConfigScene::Update()
{
	static bool configMode = false;  // 方向キーを含めた全てのキーに対応するため、「選択モード」と「キー入力モード」とを切り替える。
	if (configMode) {
		if (Input::Create().IsAnyKeyPressed() || Input::Create().IsAnyButtonPressed()) {
			menu[currentIndex].lock()->OnKeyPressed();
			for (int i = 0; i < MaxItems; i++)
				menu[i].lock()->SetActive(true);
			configMode = false;
		}
	} else {
		// 項目の選択。
		const unsigned int DelayFrames = 12;
		static unsigned int previousPressedFrame = Timer::Create().GetCountedFrames();
		if (Timer::Create().GetCountedFrames() - previousPressedFrame >= DelayFrames) {
			if (Input::Create().GetKey(Input::Commands::Up)) {
				currentIndex = MathUtils::Modulo(currentIndex - 1, MaxItems);
				previousPressedFrame = Timer::Create().GetCountedFrames();
			}
			if (Input::Create().GetKey(Input::Commands::Down)) {
				currentIndex = MathUtils::Modulo(currentIndex + 1, MaxItems);
				previousPressedFrame = Timer::Create().GetCountedFrames();
			}
		} else {
			if (Input::Create().GetKeyDown(Input::Commands::Up)) {
				currentIndex = MathUtils::Modulo(currentIndex - 1, MaxItems);
				previousPressedFrame = Timer::Create().GetCountedFrames();
			}
			if (Input::Create().GetKeyDown(Input::Commands::Down)) {
				currentIndex = MathUtils::Modulo(currentIndex + 1, MaxItems);
				previousPressedFrame = Timer::Create().GetCountedFrames();
			}
		}

		// 現在の選択肢に対する処理。
		if (currentIndex < MaxItems - 1 && Input::Create().GetKeyDown(Input::Commands::OK)) {
			for (int i = 0; i < MaxItems; i++)
				menu[i].lock()->SetActive(false);
			menu[currentIndex].lock()->SetActive(true);
			configMode = true;
		}
	}
	userInterfaceManager->Update();
	if (currentIndex == MaxItems - 1 && Input::Create().GetKeyDown(Input::Commands::OK))
		listener.PopScene();
}
