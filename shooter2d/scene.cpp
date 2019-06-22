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
{
}

void TitleScene::Draw()
{
}

void TitleScene::Update()
{
	listener.PushScene(std::make_unique<GameScene>(listener));
}

/******************************** GameOverScene *********************************/

GameOverScene::GameOverScene(IChangingSceneListener& listener)
	: Scene(listener)
{
	userInterfaceManager = std::make_unique<UserInterfaceManager>();
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::GameOver, Vector2{ Game::Width / 2.0f - 40, Game::Height / 2.0f  - 7 });
}

void GameOverScene::Draw()
{
	userInterfaceManager->Draw();
}

void GameOverScene::Update()
{
	userInterfaceManager->Update();
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
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2{ 0, Game::Height - 14 });
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
	if (player->GetLife() <= 0) {
		listener.PushScene(std::make_unique<GameOverScene>(listener));
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