#include <iostream>
#include "game.h"
#include "user_interface.h"
#include "mover.h"

using namespace Shooter;

GameScene::GameScene()
{
	tasksList.push_back(std::make_unique<FrameUI>(0, Game::Height - 14));
	tasksList.push_back(std::make_unique<Player>(Vector2{ (Game::Width - Player::Width) / 2.0f, Game::Height - Player::Height * 1.5f }, 4.0f, 2.0f));
}

void GameScene::Update()
{
	run();
	for (auto&& task : tasksList)
		task->Update();
}

void GameScene::Draw()
{
	for (auto&& task : tasksList)
		task->Draw();
}

void GameScene::run()  // 処理の全容を記述
{
	static unsigned int frames = 0;
	static auto enemy = std::make_shared<Enemy>(Vector2{ (Game::Width - Player::Width) / 2.0f, 0.0f }, Vector2{ 0.0f, 1.0f });
	++frames;
	if (frames == 1) {
		tasksList.push_back(enemy);
	} else if (frames >= 2) {
		enemy->SetVelocity({ 4.0f * std::cos(frames * Time->GetDeltaTime() * 2.0f), 1.0f });
	}
}
