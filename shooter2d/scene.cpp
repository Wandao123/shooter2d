#include "game.h"
#include "media.h"

using namespace Shooter;

/******************************** 非公開クラス *********************************/

class GameOverScene : public Scene
{
public:
	GameOverScene(IChangingSceneListener& listener, Parameters& parameters);
	void Draw() const override;
	void Update() override;
private:
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	unsigned int createdFrame;
};

class GameClearScene : public Scene
{
public:
	GameClearScene(IChangingSceneListener& listener, Parameters& parameters);
	void Draw() const override;
	void Update() override;
private:
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	unsigned int createdFrame;
};

class GameAllClearScene : public Scene
{
public:
	GameAllClearScene(IChangingSceneListener& listener, Parameters& parameters);
	void Draw() const override;
	void Update() override;
private:
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	unsigned int createdFrame;
};

class GameScene : public Scene
{
public:
	GameScene(IChangingSceneListener& listener, Parameters& parameters);
	void Draw() const override;
	void Update() override;
private:
	std::shared_ptr<EffectManager> effectManager;
	std::shared_ptr<EnemyManager> enemyManager;
	std::shared_ptr<BulletManager> enemyBulletManager;
	std::shared_ptr<PlayerManager> playerManager;
	std::shared_ptr<BulletManager> playerBulletManager;
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	std::unique_ptr<CollisionDetector> collisionDetector;
	std::unique_ptr<Script> script;
};

class KeyConfigScene : public Scene
{
public:
	KeyConfigScene(IChangingSceneListener& listener, Parameters& parameters);
	void Draw() const override;
	void Update() override;
private:
	static const int MaxItems = 10;
	const int ItemHeight = UserInterfaceManager::FontSize * 5 / 4;
	RectangleShape rectangle;
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	int currentIndex;
	std::array<std::weak_ptr<UserInterface>, MaxItems> menu;
};

class OptionsScene : public Scene
{
public:
	OptionsScene(IChangingSceneListener& listener, Parameters& parameters);
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
	PauseScene(IChangingSceneListener& listener, Parameters& parameters);
	~PauseScene();
	void Draw() const override;
	void Update() override;
private:
	static const int MaxItems = 2;
	const int ItemHeight = UserInterfaceManager::FontSize * 5 / 4;
	std::unique_ptr<Sprite> screenshot;
	std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	int currentIndex;
	std::array<std::weak_ptr<UserInterface>, MaxItems> menu;
};

class PlayerSelectScene : public Scene
{
public:
	PlayerSelectScene(IChangingSceneListener& listener, Parameters& parameters);
	void Draw() const override;
	void Update() override;
private:
	static const int MaxItems = 3;
	const int ItemHeight = UserInterfaceManager::FontSize * 5 / 4;
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

TitleScene::TitleScene(IChangingSceneListener& listener, Parameters& parameters)
	: Scene(listener, parameters)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, currentIndex(0)
{
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2<double>{ Media::Create().GetWidth() * 0.5, Media::Create().GetHeight() * 1.0 / 5.0 }).lock()->SetCaption(u8"Bullet Hell 2D Shmup");
	auto makePos = [this](unsigned int index) -> Vector2<double> { return { Media::Create().GetWidth() * 0.5, Media::Create().GetHeight() * 2.0 / 5.0 + ItemHeight * index }; };
	menu[0] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(0));
	menu[0].lock()->SetCaption(u8"Start");
	menu[1] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(1));
	menu[1].lock()->SetCaption(u8"Options");
	menu[2] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(2));
	menu[2].lock()->SetCaption(u8"Quit");
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2<double>(Media::Create().GetWidth() - 56, Media::Create().GetHeight() - 7));
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
			listener.PushScene(std::make_unique<PlayerSelectScene>(listener, parameters));
			break;
		case 1:
			listener.PushScene(std::make_unique<OptionsScene>(listener, parameters));
			break;
		case 2:
			listener.Quit();
			break;
		}
	} else if (Input::Create().GetKeyDown(Input::Commands::Cancel)) {
		if (currentIndex == 2)
			listener.Quit();
		else
			currentIndex = 2;
	}
}

/******************************** GameOverScene *********************************/

GameOverScene::GameOverScene(IChangingSceneListener& listener, Parameters& parameters)
	: Scene(listener, parameters)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, createdFrame(Timer::Create().GetPlayingFrames())
{
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2<double>{ Media::Create().GetWidth() * 0.5, Media::Create().GetHeight() * 0.5 }).lock()->SetCaption("GAME OVER");
}

void GameOverScene::Draw() const
{
	userInterfaceManager->Draw();
}

void GameOverScene::Update()
{
	userInterfaceManager->Update();
	if (Timer::Create().GetPlayingFrames() - createdFrame > Timer::FPS * 3)
		listener.ClearAndChangeScene(std::make_unique<TitleScene>(listener, parameters));
}

/******************************** GameClearScene *********************************/

GameClearScene::GameClearScene(IChangingSceneListener& listener, Parameters& parameters)
	: Scene(listener, parameters)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, createdFrame(Timer::Create().GetPlayingFrames())
{
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2<double>{ Media::Create().GetWidth() * 0.5, Media::Create().GetHeight() * 0.5 }).lock()->SetCaption("STAGE CLEAR");
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

GameAllClearScene::GameAllClearScene(IChangingSceneListener& listener, Parameters& parameters)
	: Scene(listener, parameters)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, createdFrame(Timer::Create().GetPlayingFrames())
{
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2<double>{ Media::Create().GetWidth() * 0.5, Media::Create().GetHeight() * 0.5 }).lock()->SetCaption("ALL CLEAR");
}

void GameAllClearScene::Draw() const
{
	userInterfaceManager->Draw();
}

void GameAllClearScene::Update()
{
	userInterfaceManager->Update();
	if (Timer::Create().GetPlayingFrames() - createdFrame > Timer::Create().FPS * 3)
		listener.ClearAndChangeScene(std::make_unique<TitleScene>(listener, parameters));
}

/******************************** GameScene *********************************/

GameScene::GameScene(IChangingSceneListener& listener, Parameters& parameters)
	: Scene(listener, parameters)
	, effectManager(std::make_shared<EffectManager>())
	, enemyManager(std::make_shared<EnemyManager>())
	, enemyBulletManager(std::make_shared<BulletManager>())
	, playerManager(std::make_shared<PlayerManager>())
	, playerBulletManager(std::make_shared<BulletManager>())
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, collisionDetector(std::make_unique<CollisionDetector>(*effectManager, *enemyManager, *enemyBulletManager, *playerManager, *playerBulletManager))
	, script(std::make_unique<Script>(*effectManager, *enemyManager, *enemyBulletManager, *playerManager, *playerBulletManager))
{
	// UIの初期化。
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2<double>(Media::Create().GetWidth() - 56, Media::Create().GetHeight() - 7));
	auto objectMonitor = userInterfaceManager->GenerateObject(UserInterfaceManager::StatusMonitorID::PlayersMonitor, Vector2<double>{ UserInterfaceManager::FontSize * 4, UserInterfaceManager::FontSize * 3 / 4 / 2 });
	objectMonitor.lock()->SetCaption(u8"Player");
	objectMonitor.lock()->Register(playerManager);

	// スクリプトの初期化。
	script->LoadFile("scripts/main.lua");
	script->RegisterFunction("Main");
	switch (parameters[Scene::ParameterKeys::Character]) {
	case static_cast<int>(PlayerManager::PlayerID::Reimu):
		script->LoadFile("scripts/reimu.lua");
		break;
	case static_cast<int>(PlayerManager::PlayerID::Marisa):
		script->LoadFile("scripts/marisa.lua");
		break;
	case static_cast<int>(PlayerManager::PlayerID::Sanae):
		script->LoadFile("scripts/sanae.lua");
		break;
	}
	script->RegisterFunction("PlayerScript");
}

void GameScene::Update()
{
	auto player = playerManager->GetPlayer().lock();
	auto status = script->Run();
	enemyBulletManager->Update();
	playerBulletManager->Update();
	effectManager->Update();
	enemyManager->Update();
	playerManager->Update();
	userInterfaceManager->Update();
	collisionDetector->CheckAll();

	if (Input::Create().GetKeyDown(Input::Commands::Pause))
		listener.PushScene(std::make_unique<PauseScene>(listener, parameters));
	else if (player->GetLife() <= 0)
		waitAndDo(30, [this]() { listener.PushScene(std::make_unique<GameOverScene>(listener, parameters)); });
	else
		switch (status) {
		case Script::Status::AllClear:
			listener.PushScene(std::make_unique<GameAllClearScene>(listener, parameters));
			break;
		case Script::Status::Dead:
			listener.ClearAndChangeScene(std::make_unique<TitleScene>(listener, parameters));
			break;
		case Script::Status::Running:
			break;
		case Script::Status::StageClear:
			listener.PushScene(std::make_unique<GameClearScene>(listener, parameters));
			break;
		}
}

// SDLにはZ-orderの概念が無いため、描画のタイミングで順番に注意する必要がある。
void GameScene::Draw() const
{
	userInterfaceManager->Draw();
	enemyManager->Draw();
	effectManager->Draw();
	playerManager->Draw();
	enemyBulletManager->Draw();
	playerBulletManager->Draw();
}

/******************************** KeyConfigScene *********************************/

KeyConfigScene::KeyConfigScene(IChangingSceneListener& listener, Parameters& parameters)
	: Scene(listener, parameters)
	, rectangle(Vector2<int>((UserInterfaceManager::FontSize * 3 / 4) * 9 * 3, ItemHeight))  // 1文字の幅 x 文字数 x 項目数
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, currentIndex(0)
{
	rectangle.SetBlendMode(Shape::BlendMode::Blend);
	rectangle.SetColor(0xFF, 0xFF, 0x00, 0x3F);

	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2<double>{ Media::Create().GetWidth() * 0.5, Media::Create().GetHeight() * 1.0 / 10.0 }).lock()->SetCaption(u8"Key config");
	auto makePos = [this](unsigned int index) -> Vector2<double> { return { Media::Create().GetWidth() * 0.5, Media::Create().GetHeight() * 2.0 / 10.0 + ItemHeight * index }; };
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
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2<double>(Media::Create().GetWidth() - 56, Media::Create().GetHeight() - 7));
}

void KeyConfigScene::Draw() const
{
	userInterfaceManager->Draw();
	int posX = static_cast<int>(Media::Create().GetWidth() * 0.5);
	int posY = static_cast<int>(Media::Create().GetHeight() * 2.0 / 10.0 + ItemHeight * currentIndex);
	rectangle.Draw({ posX - rectangle.GetSize().x / 2, posY - rectangle.GetSize().y / 2 });
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

OptionsScene::OptionsScene(IChangingSceneListener& listener, Parameters& parameters)
	: Scene(listener, parameters)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, currentIndex(0)
{
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2<double>{ Media::Create().GetWidth() * 0.5, Media::Create().GetHeight() * 1.0 / 4.0 }).lock()->SetCaption(u8"Options");
	auto makePos = [this](unsigned int index) -> Vector2<double> { return { Media::Create().GetWidth() * 0.5, Media::Create().GetHeight() * 2.0 / 4.0 + ItemHeight * index }; };
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
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2<double>(Media::Create().GetWidth() - 56, Media::Create().GetHeight() - 7));
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
	userInterfaceManager->Update();
	if (Input::Create().GetKeyDown(Input::Commands::OK)) {
		switch (currentIndex) {
		case 0:
		case 1:
		case 2:
			// 何もしない。
			break;
		case 3:
			listener.PushScene(std::make_unique<KeyConfigScene>(listener, parameters));
			break;
		case 4:
			listener.PopScene();
			break;
		}
	}
}

/******************************** PauseScene *********************************/

PauseScene::PauseScene(IChangingSceneListener& listener, Parameters& parameters)
	: Scene(listener, parameters)
	, screenshot(std::make_unique<Sprite>(AssetLoader::Create().TakeScreenshot()))
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, currentIndex(0)
{
	// TODO: Blurring effect.
	screenshot->SetColor(0x86, 0x5D, 0x36);

	// ゲームオブジェクトの生成。
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2<double>{ Media::Create().GetWidth() * 0.5, Media::Create().GetHeight() * 1.0 / 3.0 }).lock()->SetCaption(u8"Pause");
	auto makePos = [this](unsigned int index) -> Vector2<double> { return { Media::Create().GetWidth() * 0.5, Media::Create().GetHeight() * 1.0 / 2.0 + ItemHeight * index }; };
	menu[0] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(3));
	menu[0].lock()->SetCaption(u8"Return");
	menu[1] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(4));
	menu[1].lock()->SetCaption(u8"Exit");

	// 停止モードに切替。
	Timer::Create().Stop();
}

PauseScene::~PauseScene()
{
	AssetLoader::Create().ClearScreenshots();
	Timer::Create().Restart();
}

void PauseScene::Draw() const
{
	screenshot->Draw();
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
			listener.ClearAndChangeScene(std::make_unique<TitleScene>(listener, parameters));
			break;
		}
	else if (Input::Create().GetKeyDown(Input::Commands::Pause))
		listener.PopScene();
	else
		adjustWithKeys(
			Input::Commands::Up, [this](void) { currentIndex = MathUtils::Modulo(currentIndex - 1, MaxItems); },
			Input::Commands::Down, [this](void) { currentIndex = MathUtils::Modulo(currentIndex + 1, MaxItems); });
}

/******************************** PlayerSelectScene *********************************/

PlayerSelectScene::PlayerSelectScene(IChangingSceneListener& listener, Parameters& parameters)
	: Scene(listener, parameters)
	, userInterfaceManager(std::make_unique<UserInterfaceManager>())
	, currentIndex(0)
{
	auto iter = parameters.find(Scene::ParameterKeys::Character);
	if (iter != parameters.end())
		currentIndex = iter->second;

	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Title, Vector2<double>{ Media::Create().GetWidth() * 0.5, Media::Create().GetHeight() * 1.0 / 4.0 }).lock()->SetCaption(u8"Player Select");
	auto makePos = [this](unsigned int index) -> Vector2<double> { return { Media::Create().GetWidth() * 0.5, Media::Create().GetHeight() * 2.0 / 4.0 + ItemHeight * index }; };
	menu[0] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(0));
	menu[0].lock()->SetCaption(u8"Hakurei Reimu");
	menu[1] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(1));
	menu[1].lock()->SetCaption(u8"Kirisame Marisa");
	menu[2] = userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::Button, makePos(2));
	menu[2].lock()->SetCaption(u8"Kochiya Sanae");
	userInterfaceManager->GenerateObject(UserInterfaceManager::UserInterfaceID::FrameRate, Vector2<double>(Media::Create().GetWidth() - 56, Media::Create().GetHeight() - 7));
}

void PlayerSelectScene::Draw() const
{
	userInterfaceManager->Draw();
}

void PlayerSelectScene::Update()
{
	adjustWithKeys(
		Input::Commands::Up, [this](void) { currentIndex = MathUtils::Modulo(currentIndex - 1, MaxItems); },
		Input::Commands::Down, [this](void) { currentIndex = MathUtils::Modulo(currentIndex + 1, MaxItems); });
	for (int i = 0; i < MaxItems; i++)
		menu[i].lock()->SetActive((i != currentIndex) ? false : true);
	userInterfaceManager->Update();
	if (Input::Create().GetKeyDown(Input::Commands::OK) || Input::Create().GetKeyDown(Input::Commands::Cancel)) {
		switch (currentIndex) {
		case 0:
			parameters[Scene::ParameterKeys::Character] = static_cast<int>(PlayerManager::PlayerID::Reimu);
			break;
		case 1:
			parameters[Scene::ParameterKeys::Character] = static_cast<int>(PlayerManager::PlayerID::Marisa);
			break;
		case 2:
			parameters[Scene::ParameterKeys::Character] = static_cast<int>(PlayerManager::PlayerID::Sanae);
			break;
		}
		if (Input::Create().GetKeyDown(Input::Commands::OK))
			listener.ClearAndChangeScene(std::make_unique<GameScene>(listener, parameters));
		else
			listener.PopScene();
	}
}