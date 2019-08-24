#include "user_interface.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace Shooter;

#ifdef _WIN64
const std::string Filename = "C:/Windows/Fonts/consola.ttf";
#elif __linux__
const std::string Filename = "/usr/share/fonts/TTF/LiberationMono-Regular.ttf";
#elif __unix__
#else
// TODO: 確実に存在するパス？
#endif

/******************************** 個別設定用クラス *********************************/

class FrameRate : public UserInterface
{
public:
	FrameRate(const Vector2& position)
		: UserInterface(position)
		, label(std::make_unique<Label>(AssetLoader::Create().GetFont(Filename, UserInterfaceManager::FontSize / 2)))
	{}

	void Draw() const override
	{
		label->Write(position);
	}

	void Update() override
	{
		if (Timer::Create().GetCountedFrames() % (Timer::FPS / 2) == 0) {
			text.str("");
			text << "FPS " << std::fixed << std::setprecision(3) << Timer::Create().GetAverageOfFPS();
			label->Text = text.str();
		}
	}
private:
	std::unique_ptr<Label> label;
	std::stringstream text;
};

class Title : public UserInterface
{
public:
	Title(const Vector2& position)
		: UserInterface(position)
		, label(std::make_unique<Label>(AssetLoader::Create().GetFont(Filename, UserInterfaceManager::FontSize * 3 / 2)))
	{
		label->SetTextColor(0xFF, 0xFF, 0xFF);
	}

	void Draw() const override
	{
		label->Write(position);
	}

	void Update() override
	{
		UserInterface::Update();
		label->Text = caption;
	}
private:
	std::unique_ptr<Label> label;
};

class Button : public UserInterface
{
public:
	Button(const Vector2& position)
		: UserInterface(position)
		, label(std::make_unique<Label>(AssetLoader::Create().GetFont(Filename, UserInterfaceManager::FontSize)))
	{
		label->SetTextColor(0xFF, 0xFF, 0xFF);
	}

	void Draw() const override
	{
		label->Write(position);
	}

	void Update() override
	{
		if (activated) {
			label->SetTextColor(255, 255, 255);
			label->SetAlpha(255);
		} else {
			//label->SetTextColor(127, 127, 127);
			label->SetAlpha(127);
		}
		label->Text = caption;
	}
private:
	std::unique_ptr<Label> label;
};

class Potentiometer : public UserInterface
{
public:
	Potentiometer(const Vector2& position)
		: UserInterface(position)
		, label(std::make_unique<Label>(AssetLoader::Create().GetFont(Filename, UserInterfaceManager::FontSize)))
	{
		label->SetTextColor(0xFF, 0xFF, 0xFF);
	}

	void Draw() const override
	{
		label->Write(position);
	}

	void Update() override
	{
		if (activated) {
			label->SetTextColor(255, 255, 255);
			label->SetAlpha(255);
		} else {
			//label->SetTextColor(127, 127, 127);
			label->SetAlpha(127);
		}
		text.str("");
		text << std::setw(ItemWidth) << std::left << caption;
		printValue();
		label->Text = text.str();
	}

	void OnKeyPressed(Input::Commands command) override
	{
		switch (command) {
		case Input::Commands::Left:
			decrease();
			break;
		case Input::Commands::Right:
			increase();
			break;
		default:
			// 何もしない。
			break;
		}
	}
protected:
	const unsigned int ItemWidth = 14;
	std::unique_ptr<Label> label;
	std::stringstream text;
	virtual void printValue() = 0;
	virtual void decrease() = 0;
	virtual void increase() = 0;
};

class SoundVolume : public Potentiometer
{
public:
	SoundVolume(const Vector2& position)
		: Potentiometer(position)
		, value(AssetLoader::Create().GetSoundVolume())
	{}
protected:
	void printValue() override
	{
		text << std::setw(ItemWidth) << std::left << "<= " + std::to_string(value) + " =>";
	}

	void decrease() override
	{
		value = std::max(AssetLoader::Create().GetSoundVolume() - 5, 0);
		AssetLoader::Create().SetSoundVolume(value);
	}

	void increase() override
	{
		value = std::min(AssetLoader::Create().GetSoundVolume() + 5, 100);
		AssetLoader::Create().SetSoundVolume(value);
	}
private:
	int value;
};

class MusicVolume : public Potentiometer
{
public:
	MusicVolume(const Vector2& position)
		: Potentiometer(position)
		, value(AssetLoader::Create().GetMusicVolume())
	{}
protected:
	void printValue() override
	{
		text << std::setw(ItemWidth) << std::left << "<= " + std::to_string(value) + " =>";
	}

	void decrease() override
	{
		value = std::max(AssetLoader::Create().GetMusicVolume() - 5, 0);
		AssetLoader::Create().SetMusicVolume(value);
	}

	void increase() override
	{
		value = std::min(AssetLoader::Create().GetMusicVolume() + 5, 100);
		AssetLoader::Create().SetMusicVolume(value);
	}
private:
	int value;
};

namespace Shooter {
	extern SDL_Window* Window;
}

class FullscreenSwitcher : public Potentiometer
{
public:
	FullscreenSwitcher(const Vector2& position)
		: Potentiometer(position)
		, isFullscreen((SDL_GetWindowFlags(Window) & SDL_WINDOW_FULLSCREEN) != 0)
	{}
protected:
	void printValue() override
	{
		text << std::setw(ItemWidth) << std::left << (isFullscreen ? "ON" : "OFF");
	}

	void decrease() override
	{
		// HACK: Window変数の依存性をGameクラスに押しつけられないか？
		if (isFullscreen) {
			isFullscreen = false;
			SDL_SetWindowFullscreen(Window, 0);
		} else {
			isFullscreen = true;
			SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN);
		}
	}

	void increase() override
	{
		decrease();
	}
private:
	bool isFullscreen;
};

template<Input::Commands Command>
class KeyConfig : public UserInterface
{
public:
	KeyConfig(const Vector2& position)
		: UserInterface(position)
		, label(std::make_unique<Label>(AssetLoader::Create().GetFont(Filename, UserInterfaceManager::FontSize * 3 / 4)))
	{
		label->SetTextColor(0xFF, 0xFF, 0xFF);
	}

	void Draw() const override
	{
		label->Write(position);
	}

	void Update() override
	{
		if (activated) {
			label->SetTextColor(255, 255, 255);
			label->SetAlpha(255);
		} else {
			//label->SetTextColor(127, 127, 127);
			label->SetAlpha(127);
		}
		text.str("");
		text << std::setw(ItemWidth) << std::left << caption;
		text << std::setw(ItemWidth) << std::left << Input::Create().GetCurrentKeyNameFor(Command);
		text << std::setw(ItemWidth) << std::left << Input::Create().GetCurrentButtonNameFor(Command);
		label->Text = text.str();
	}

	void OnKeyPressed() override
	{
		Input::Create().TranslateKeyInto(Command);
		Input::Create().TranslateButtonInto(Command);
	}
private:
	const unsigned int ItemWidth = 14;
	std::unique_ptr<Label> label;
	std::stringstream text;
};

/******************************** UserInterface *********************************/

UserInterface::UserInterface(const Vector2& position)
	: GameObject(true, position)
	, caption(" ")
	, activated(true)
{}

void UserInterface::Update()
{
	if (!activated)
		return;
}

/******************************** UserInterfaceManager *********************************/

std::weak_ptr<UserInterface> UserInterfaceManager::GenerateObject(const UserInterfaceID id, const Vector2& position)
{
	std::weak_ptr<UserInterface> newObject;
	switch (id) {
	case UserInterfaceID::FrameRate:
		newObject = assignObject<FrameRate>(position);
		break;
	case UserInterfaceID::Title:
		newObject = assignObject<Title>(position);
		break;
	case UserInterfaceID::Button:
		newObject = assignObject<Button>(position);
		break;
	case UserInterfaceID::SoundVolume:
		newObject = assignObject<SoundVolume>(position);
		break;
	case UserInterfaceID::MusicVolume:
		newObject = assignObject<MusicVolume>(position);
		break;
	case UserInterfaceID::FullscreenSwitcher:
		newObject = assignObject<FullscreenSwitcher>(position);
		break;
	case UserInterfaceID::ShotKeyConfig:
		newObject = assignObject<KeyConfig<Input::Commands::Shot>>(position);
		break;
	case UserInterfaceID::BombKeyConfig:
		newObject = assignObject<KeyConfig<Input::Commands::Bomb>>(position);
		break;
	case UserInterfaceID::SlowKeyConfig:
		newObject = assignObject<KeyConfig<Input::Commands::Slow>>(position);
		break;
	case UserInterfaceID::SkipKeyConfig:
		newObject = assignObject<KeyConfig<Input::Commands::Skip>>(position);
		break;
	case UserInterfaceID::LeftwardKeyConfig:
		newObject = assignObject<KeyConfig<Input::Commands::Leftward>>(position);
		break;
	case UserInterfaceID::RightwardKeyConfig:
		newObject = assignObject<KeyConfig<Input::Commands::Rightward>>(position);
		break;
	case UserInterfaceID::ForwardKeyConfig:
		newObject = assignObject<KeyConfig<Input::Commands::Forward>>(position);
		break;
	case UserInterfaceID::BackwardKeyConfig:
		newObject = assignObject<KeyConfig<Input::Commands::Backward>>(position);
		break;
	case UserInterfaceID::PauseKeyConfig:
		newObject = assignObject<KeyConfig<Input::Commands::Pause>>(position);
		break;
	}
	return newObject;
}
