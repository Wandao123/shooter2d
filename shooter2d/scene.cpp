#include <iostream>
#include "game.h"

using namespace Shooter;

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

GameScene::GameScene(IChangingSceneListener& listener)
	: Scene(listener)
{
	// 更新対象オブジェクトを生成。
	playerManager = std::make_shared<PlayerManager>();
	player = playerManager->GenerateObject(PlayerManager::PlayerID::Reimu, Vector2{ Game::Width / 2.0f, Game::Height - Player::Height });
	player->Spawned();
	userInterfaceManager = std::make_shared<UserInterfaceManager>();
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2{ 0, Game::Height - 14 });
	enemyManager = std::make_shared<EnemyManager>();
	bulletManager = std::make_shared<BulletManager>();
	effectManager = std::make_shared<EffectManager>();
	collisionDetector = std::make_unique<CollisionDetector>(playerManager, enemyManager, bulletManager, effectManager);
	script = std::make_unique<Script>(*bulletManager, *enemyManager, *playerManager);
}

void GameScene::Update()
{
	static int counterFromDefeated = 0;
	const int DelayFrames = 30;

	script->Run();
	if (!player->IsEnabled()) {
		++counterFromDefeated;
		if (counterFromDefeated > DelayFrames) {
			player->SetPosition(Vector2{ Game::Width / 2.0f, Game::Height - Player::Height });
			player->Spawned();
			counterFromDefeated = 0;
		}
	}
	playerManager->Update();
	userInterfaceManager->Update();
	enemyManager->Update();
	bulletManager->Update();
	effectManager->Update();
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