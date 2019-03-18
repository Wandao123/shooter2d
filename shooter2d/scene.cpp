#include "game.h"
#include "user_interface.h"
#include "mover.h"

using namespace Shooter;

GameScene::GameScene()
{
	tasksList.push_back(std::make_unique<FrameUI>(0, Game::Height - 14));
	Vector2 position{ (Game::Width - Player::Width) / 2.0f, Game::Height - Player::Height * 1.5f };
	tasksList.push_back(std::make_unique<Player>(position, 4.0f, 2.0f));
	position = { (Game::Width - Player::Width) / 2.0f, 0.0f };
	Vector2 velocity{ 0.0f, 1.0f };
	tasksList.push_back(std::make_unique<Enemy>(position, velocity));
}

void GameScene::Update()
{
	for (auto&& task : tasksList)
		task->Update();
}

void GameScene::Draw()
{
	for (auto&& task : tasksList)
		task->Draw();
}
