#include "game.h"
#include "user_interface.h"
#include "mover.h"

using namespace Shooter;

GameScene::GameScene()
{
	tasksList.push_back(std::make_unique<FrameUI>(0, Game::Height - 14));
	Vector2 position{ (Game::Width - Player::Width) / 2.0f, Game::Height - Player::Height * 1.5f };
	tasksList.push_back(std::make_unique<Player>(position, 4.0f, 2.0f));
}

void GameScene::Update()
{
	for (auto iter = tasksList.begin(); iter != tasksList.end(); iter++)
		(*iter)->Update();
}

void GameScene::Draw()
{
	for (auto iter = tasksList.begin(); iter != tasksList.end(); iter++)
		(*iter)->Draw();
}