#include <iostream>
#include "game.h"

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
}

void TitleScene::Draw()
{
	userInterfaceManager->Draw();
}

void TitleScene::Update()
{
	// TODO: 入力処理を別クラスへ分離。
	static const int DelayFrames = 10;
	static Uint32 keyDownFrame = 0;  // 多重入力禁止用のフラグ
	const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
	if (Time->GetCountedFrames() - keyDownFrame > DelayFrames) {
		if (currentKeyStates[SDL_SCANCODE_UP]) {
			menu.lock()->Up();
			keyDownFrame = Time->GetCountedFrames();
		}
		else if (currentKeyStates[SDL_SCANCODE_DOWN]) {
			menu.lock()->Down();
			keyDownFrame = Time->GetCountedFrames();
		}
	}
	userInterfaceManager->Update();  // ClearAndChangeSceneを実行してから更新するとエラー。
	if (currentKeyStates[SDL_SCANCODE_Z]) {
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
	, createdFrame(Time->GetCountedFrames())
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
	if (Time->GetCountedFrames() - createdFrame > Time->FPS * 3)
		listener.ClearAndChangeScene(std::make_unique<TitleScene>(listener));
}

/******************************** GameClearScene *********************************/

GameClearScene::GameClearScene(IChangingSceneListener& listener)
	: Scene(listener)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, createdFrame(Time->GetCountedFrames())
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
	if (Time->GetCountedFrames() - createdFrame > Time->FPS * 3)
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
}

void GameScene::Update()
{
	auto updatePlayer = [this](Player& player) {
		// 自機の復活処理。
		static int counterFromDefeated = 0;
		const int DelayFrames = 30;
		if (!player.IsEnabled()) {
			++counterFromDefeated;
			if (counterFromDefeated > DelayFrames) {
				player.SetPosition(Vector2{ Game::Width / 2.0f, Game::Height - Player::Height });
				player.Spawned();
				counterFromDefeated = 0;
			}
		}
		
		// 自機の入力処理。
		// TODO: 別クラスへ分離。
		float speed;
		const SDL_Keymod modStates = SDL_GetModState();

		if (modStates & KMOD_SHIFT)
			speed = player.GetLowSpeed();
		else
			speed = player.GetHighSpeed();
		Vector2 velocity = { 0.0f, 0.0f };
		const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
		if (currentKeyStates[SDL_SCANCODE_LEFT])
			velocity.x = -speed;
		if (currentKeyStates[SDL_SCANCODE_RIGHT])
			velocity.x = +speed;
		if (currentKeyStates[SDL_SCANCODE_UP])
			velocity.y = -speed;
		if (currentKeyStates[SDL_SCANCODE_DOWN])
			velocity.y = +speed;
		velocity = velocity.Normalize() * speed;
		player.SetVelocity(velocity);

		if (currentKeyStates[SDL_SCANCODE_Z])
			player.Shoot();
	};

	auto player = playerManager->GetPlayer().lock();
	updatePlayer(*player);
	script->Run();
	bulletManager->Update();
	effectManager->Update();
	enemyManager->Update();
	playerManager->Update();
	userInterfaceManager->Update();
	collisionDetector->CheckAll();
	
	if (player->GetLife() <= 0)
		listener.PushScene(std::make_unique<GameOverScene>(listener));
	if (script->IsTerminated())  // HACK: スクリプトに無限ループが紛れ込んでしまったら、これだと対処できない。スクリプトで実行できるように、オブジェクトを全て削除する命令、あるいはシーンの切り替え命令を実装するべき？
		listener.PushScene(std::make_unique<GameClearScene>(listener));
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
