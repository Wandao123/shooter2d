#include "game.h"
#include "input.h"

using namespace Shooter;

/******************************** 非公開クラス *********************************/

class GameOverScene : public Scene
{
public:
	GameOverScene(IChangingSceneListener& listener);
	void Draw() override;
	void Update() override;
private:
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	unsigned int createdFrame;
};

class GameClearScene : public Scene
{
public:
	GameClearScene(IChangingSceneListener& listener);
	void Draw() override;
	void Update() override;
private:
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	unsigned int createdFrame;
};

class GameAllClearScene : public Scene
{
public:
	GameAllClearScene(IChangingSceneListener& listener);
	void Draw() override;
	void Update() override;
private:
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	unsigned int createdFrame;
};

class GameScene : public Scene
{
public:
	GameScene(IChangingSceneListener& listener);
	void Draw() override;
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

/******************************** TitleScene *********************************/

TitleScene::TitleScene(IChangingSceneListener& listener)
	: Scene(listener)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
{
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2{ Game::Width * 0.5f, Game::Height * 1.0f / 5.0f });
	menu = userInterfaceManager->GenerateObject(UserInterfaceManager::MenuID::Title, Vector2{ Game::Width * 0.5f, Game::Height * 2.0f / 5.0f });
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2{ Game::Width - 56, Game::Height - 7 });
}

void TitleScene::Draw()
{
	userInterfaceManager->Draw();
}

void TitleScene::Update()
{
	if (Input::Create().GetKeyDown(Input::Commands::Forward))
		menu.lock()->Up();
	else if (Input::Create().GetKeyDown(Input::Commands::Backward))
		menu.lock()->Down();
	userInterfaceManager->Update();  // ClearAndChangeSceneを実行してから更新するとエラー。
	if (Input::Create().GetKeyDown(Input::Commands::Shot)) {
		switch (menu.lock()->GetCurrentItemIndex()) {
		case 0:
			listener.ClearAndChangeScene(std::make_unique<GameScene>(listener));
			break;
		case 1:
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
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::GameOver, Vector2{ Game::Width * 0.5f, Game::Height * 0.5f });
}

void GameOverScene::Draw()
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
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::GameClear, Vector2{ Game::Width * 0.5f, Game::Height * 0.5f });
}

void GameClearScene::Draw()
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
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::GameAllClear, Vector2{ Game::Width * 0.5f, Game::Height * 0.5f });
}

void GameAllClearScene::Draw()
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
void GameScene::Draw()
{
	userInterfaceManager->Draw();
	effectManager->Draw();
	playerManager->Draw();
	enemyManager->Draw();
	bulletManager->Draw();
}
