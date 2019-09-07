#include "game.h"

// TODO: assetsに押し付ける方法？
namespace Shooter {
	extern SDL_Renderer* Renderer;
}


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
	const int ItemHeight = UserInterfaceManager::FontSize * 5 / 4;
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	int currentIndex;
	std::array<std::weak_ptr<UserInterface>, MaxItems> menu;
};

class OptionsScene : public Scene
{
public:
	OptionsScene(IChangingSceneListener& listener);
	void Draw() const override;
	void Update() override;
private:
	static const int MaxItems = 5;
	const int ItemHeight = UserInterfaceManager::FontSize * 5 / 4;
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	int currentIndex;
	std::array<std::weak_ptr<UserInterface>, MaxItems> menu;
};

class PauseScene : public Scene
{
public:
	PauseScene(IChangingSceneListener& listener);
	~PauseScene();
	void Draw() const override;
	void Update() override;
private:
	static const int MaxItems = 2;
	const int ItemHeight = UserInterfaceManager::FontSize * 5 / 4;
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> screenshot;
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	int currentIndex;
	std::array<std::weak_ptr<UserInterface>, MaxItems> menu;
};

/******************************** Scene *********************************/

void Scene::adjustWithKeys(Input::Commands decrement, std::function<void(void)> decrease, Input::Commands increment, std::function<void(void)> increase)
{
	const unsigned int DelayFrames = 12;
	if (Timer::Create().GetPlayingFrames() - previousPressedFrame >= DelayFrames) {
		if (Input::Create().GetKey(decrement)) {
			decrease();
			previousPressedFrame = Timer::Create().GetPlayingFrames();
		}
		if (Input::Create().GetKey(increment)) {
			increase();
			previousPressedFrame = Timer::Create().GetPlayingFrames();
		}
	} else {
		if (Input::Create().GetKeyDown(decrement)) {
			decrease();
			previousPressedFrame = Timer::Create().GetPlayingFrames();
		}
		if (Input::Create().GetKeyDown(increment)) {
			increase();
			previousPressedFrame = Timer::Create().GetPlayingFrames();
		}
	}
}

void Scene::waitAndDo(const unsigned int delayFrames, std::function<void(void)> func)
{
	if (counter < delayFrames) {
		++counter;
	} else {
		counter = 0;
		func();
	}
}


/******************************** TitleScene *********************************/

TitleScene::TitleScene(IChangingSceneListener& listener)
	: Scene(listener)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, currentIndex(0)
{
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2{ Game::Width * 0.5f, Game::Height * 1.0f / 5.0f }).lock()->SetCaption(u8"Bullet Hell 2D Shmup");
	auto makePos = [this](unsigned int index) -> Vector2 { return { Game::Width * 0.5f, Game::Height * 2.0f / 5.0f + ItemHeight * index }; };
	menu[0] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(0));
	menu[0].lock()->SetCaption(u8"Start");
	menu[1] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(1));
	menu[1].lock()->SetCaption(u8"Options");
	menu[2] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(2));
	menu[2].lock()->SetCaption(u8"Quit");
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2{ Game::Width - 56, Game::Height - 7 });
}

void TitleScene::Draw() const
{
	userInterfaceManager->Draw();
}

void TitleScene::Update()
{
	adjustWithKeys(
		Input::Commands::Up, [this](void) { currentIndex = MathUtils::Modulo(currentIndex - 1, MaxItems); },
		Input::Commands::Down, [this](void) { currentIndex = MathUtils::Modulo(currentIndex + 1, MaxItems); });
	for (int i = 0; i < MaxItems; i++)
		menu[i].lock()->SetActive((i != currentIndex) ? false : true);
	userInterfaceManager->Update();  // ClearAndChangeSceneを実行してから更新するとエラー。
	if (Input::Create().GetKeyDown(Input::Commands::OK)) {
		switch (currentIndex) {
		case 0:
			listener.ClearAndChangeScene(std::make_unique<GameScene>(listener));
			break;
		case 1:
			listener.PushScene(std::make_unique<OptionsScene>(listener));
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
	, createdFrame(Timer::Create().GetPlayingFrames())
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
	if (Timer::Create().GetPlayingFrames() - createdFrame > Timer::FPS * 3)
		listener.ClearAndChangeScene(std::make_unique<TitleScene>(listener));
}

/******************************** GameClearScene *********************************/

GameClearScene::GameClearScene(IChangingSceneListener& listener)
	: Scene(listener)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, createdFrame(Timer::Create().GetPlayingFrames())
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
	if (Timer::Create().GetPlayingFrames() - createdFrame > Timer::Create().FPS * 3)
		listener.PopScene();
}

/******************************** GameAllClearScene *********************************/

GameAllClearScene::GameAllClearScene(IChangingSceneListener& listener)
	: Scene(listener)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, createdFrame(Timer::Create().GetPlayingFrames())
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
	if (Timer::Create().GetPlayingFrames() - createdFrame > Timer::Create().FPS * 3)
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

	auto objectMonitor = userInterfaceManager->GenerateObject(UserInterfaceManager::StatusMonitorID::PlayersMonitor, Vector2{ UserInterfaceManager::FontSize * 4, UserInterfaceManager::FontSize * 3 / 4 / 2 });
	objectMonitor.lock()->SetCaption("Player");
	objectMonitor.lock()->Register(playerManager);
}

void GameScene::Update()
{
	auto updatePlayer = [this](Player& player) {
		// 自機の復活処理。
		const unsigned int DelayFrames = 30;
		static unsigned int playerDefeatedFrame = Timer::Create().GetPlayingFrames();
		if (!player.IsEnabled() && player.GetLife() > 0 && Timer::Create().GetPlayingFrames() - playerDefeatedFrame >= DelayFrames) {
			player.SetPosition(Vector2{ Game::Width / 2.0f, Game::Height - Player::Height });
			player.Spawned();
			playerDefeatedFrame = Timer::Create().GetPlayingFrames();
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
			static unsigned int playerShootingFrame = Timer::Create().GetPlayingFrames();
			if (Timer::Create().GetPlayingFrames() - playerShootingFrame >= ShotDelayFrames) {
				player.Shoot();
				playerShootingFrame = Timer::Create().GetPlayingFrames();
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

	if (Input::Create().GetKeyDown(Input::Commands::Pause))
		listener.PushScene(std::make_unique<PauseScene>(listener));
	else if (player->GetLife() <= 0)
		waitAndDo(30, [this]() { listener.PushScene(std::make_unique<GameOverScene>(listener)); });
	else
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

void KeyConfigScene::Draw() const
{
	userInterfaceManager->Draw();

	// TODO: 選択中のUIを実装して、Rendererを隠蔽。
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
		adjustWithKeys(
			Input::Commands::Up, [this](void) { currentIndex = MathUtils::Modulo(currentIndex - 1, MaxItems); },
			Input::Commands::Down, [this](void) { currentIndex = MathUtils::Modulo(currentIndex + 1, MaxItems); });
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

/******************************** OptionsScene *********************************/

OptionsScene::OptionsScene(IChangingSceneListener& listener)
	: Scene(listener)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, currentIndex(0)
{
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2{ Game::Width * 0.5f, Game::Height * 1.0f / 4.0f }).lock()->SetCaption(u8"Options");
	auto makePos = [this](unsigned int index) -> Vector2 { return { Game::Width * 0.5f, Game::Height * 2.0f / 4.0f + ItemHeight * index }; };
	menu[0] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::MusicVolume, makePos(0));
	menu[0].lock()->SetCaption(u8"Music Volume");
	menu[1] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::SoundVolume, makePos(1));
	menu[1].lock()->SetCaption(u8"SE Volume");
	menu[2] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FullscreenSwitcher, makePos(2));
	menu[2].lock()->SetCaption(u8"Fullscreen");
	menu[3] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(3));
	menu[3].lock()->SetCaption(u8"Key config");
	menu[4] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(4));
	menu[4].lock()->SetCaption(u8"Quit");
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2{ Game::Width - 56, Game::Height - 7 });
}

void OptionsScene::Draw() const
{
	userInterfaceManager->Draw();
}

void OptionsScene::Update()
{
	adjustWithKeys(
		Input::Commands::Up, [this](void) { currentIndex = MathUtils::Modulo(currentIndex - 1, MaxItems); },
		Input::Commands::Down, [this](void) { currentIndex = MathUtils::Modulo(currentIndex + 1, MaxItems); });
	for (int i = 0; i < MaxItems; i++)
		menu[i].lock()->SetActive((i != currentIndex) ? false : true);
	adjustWithKeys(
		Input::Commands::Right, [this](void) { menu[currentIndex].lock()->OnKeyPressed(Input::Commands::Right); },
		Input::Commands::Left, [this](void) { menu[currentIndex].lock()->OnKeyPressed(Input::Commands::Left); });
	userInterfaceManager->Update();  // ClearAndChangeSceneを実行してから更新するとエラー。
	if (Input::Create().GetKeyDown(Input::Commands::OK)) {
		switch (currentIndex) {
		case 0:
		case 1:
		case 2:
			// 何もしない。
			break;
		case 3:
			listener.PushScene(std::make_unique<KeyConfigScene>(listener));
			break;
		case 4:
			listener.PopScene();
			break;
		}
	}
}

/******************************** PauseScene *********************************/

PauseScene::PauseScene(IChangingSceneListener& listener)
	: Scene(listener)
	, screenshot(nullptr, nullptr)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, currentIndex(0)
{
	// スクリーンショットの保存。
	auto surface = SDL_CreateRGBSurface(0, Game::Width, Game::Height, 32, 0, 0, 0, 0);
	SDL_RenderReadPixels(Renderer, nullptr, surface->format->format, surface->pixels, surface->pitch);
	auto rawTexture = SDL_CreateTextureFromSurface(Renderer, surface);
	SDL_SetTextureColorMod(rawTexture, 0x86, 0x5D, 0x36);
	// TODO: Blurring effect.
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture(rawTexture, SDL_DestroyTexture);
	this->screenshot = std::move(texture);
	SDL_FreeSurface(surface);

	// ゲームオブジェクトの生成。
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2{ Game::Width * 0.5f, Game::Height * 1.0f / 3.0f }).lock()->SetCaption(u8"Pause");
	auto makePos = [this](unsigned int index) -> Vector2 { return { Game::Width * 0.5f, Game::Height * 1.0f / 2.0f + ItemHeight * index }; };
	menu[0] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(3));
	menu[0].lock()->SetCaption(u8"Return");
	menu[1] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(4));
	menu[1].lock()->SetCaption(u8"Exit");

	// 停止モードに切替。
	Timer::Create().Stop();
}

PauseScene::~PauseScene()
{
	Timer::Create().Restart();
}

void PauseScene::Draw() const
{
	SDL_RenderCopy(Renderer, screenshot.get(), nullptr, nullptr);
	userInterfaceManager->Draw();
}

void PauseScene::Update()
{
	for (int i = 0; i < MaxItems; i++)
		menu[i].lock()->SetActive((i != currentIndex) ? false : true);
	userInterfaceManager->Update();
	if (Input::Create().GetKeyDown(Input::Commands::OK))
		switch (currentIndex) {
		case 0:
			listener.PopScene();
			break;
		case 1:
			listener.ClearAndChangeScene(std::make_unique<TitleScene>(listener));
			break;
		}
	else if (Input::Create().GetKeyDown(Input::Commands::Pause))
		listener.PopScene();
	else
		adjustWithKeys(
			Input::Commands::Up, [this](void) { currentIndex = MathUtils::Modulo(currentIndex - 1, MaxItems); },
			Input::Commands::Down, [this](void) { currentIndex = MathUtils::Modulo(currentIndex + 1, MaxItems); });
}
