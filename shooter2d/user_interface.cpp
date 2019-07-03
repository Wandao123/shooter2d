#include <iostream>
#include <iomanip>
#include <array>
#include "user_interface.h"

using namespace Shooter;

#ifdef _WIN64
const std::string Filename = "C:/Windows/Fonts/arial.ttf";
#elif __linux__
const std::string Filename = "/usr/share/fonts/TTF/LiberationSans-Regular.ttf";
#elif __unix__
#else
// TODO: 確実に存在するパス？
#endif
const int FontSize = 28;

/******************************** 個別設定用クラス *********************************/

class FrameRate : public UserInterface
{
public:
	FrameRate(const Vector2& position) : UserInterface(position), label(std::make_unique<Label>(Filename, FontSize / 2)) {}

	void Draw() const override
	{
		label->Write(position);
	}

	void Update() override
	{
		if (Time->GetCountedFrames() % (Time->FPS / 2) == 0) {
			label->Text.str("");
			label->Text << "FPS " << std::fixed << std::setprecision(3) << Time->GetAverageOfFPS();
		}
	}
private:
	std::unique_ptr<Label> label;
};

class Title : public UserInterface
{
public:
	Title(const Vector2& position)
		: UserInterface(position)
		, label(std::make_unique<Label>(Filename, FontSize + FontSize / 2))
	{
		label->Text.str(u8"2D Bullet Hell Shooter");
	}

	void Draw() const override
	{
		label->Write(position);
	}

	void Update() override
	{

	}

private:
	std::unique_ptr<Label> label;
};

class TitleMenu : public UserInterface, public IMenu
{
public:
	TitleMenu(const Vector2& position)
		: UserInterface(position)
		, currentItemIndex(0)
	{
		for (int i = 0; i < MaxItems; i++)
			items[i] = std::make_unique<Label>(Filename, FontSize);
		items[0]->Text.str("Start");
		items[1]->Text.str("Quit");
	}

	void Draw() const override
	{
		constexpr float startingPointY = (MaxItems % 2 == 0) ? (MaxItems / 2 - 0.5f) * LineHeight : MaxItems / 2 * LineHeight;
		for (int i = 0; i < MaxItems; i++)
			items[i]->Write(position + Vector2{ 0.0f, startingPointY + i * LineHeight });
	}

	void Update() override
	{
		for (int i = 0; i < MaxItems; i++) {
			if (i != currentItemIndex) {
				items[i]->SetTextColor(127, 127, 127);
				items[i]->SetAlpha(127);
			} else {
				items[i]->SetTextColor(255, 255, 255);
				items[i]->SetAlpha(255);
			}
		}
	}

	int GetCurrentItemIndex() override
	{
		return currentItemIndex;
	}

	void Up() override
	{
		currentItemIndex = MathUtils::Modulo(currentItemIndex + 1, MaxItems);
	}

	void Down() override
	{
		currentItemIndex = MathUtils::Modulo(currentItemIndex - 1, MaxItems);
	}
private:
	static constexpr int LineHeight = FontSize + FontSize / 4;
	static constexpr int MaxItems = 2;
	int currentItemIndex;
	std::array<std::unique_ptr<Label>, MaxItems> items;
};

class GameOver : public UserInterface
{
public:
	GameOver(const Vector2& position)
		: UserInterface(position)
		, label(std::make_unique<Label>(Filename, FontSize))
	{
		label->Text.str("GAME OVER");
	}
	
	void Draw() const override
	{
		label->Write(position);
	}
	
	void Update() override
	{

	}
private:
	std::unique_ptr<Label> label;
};

class GameClear : public UserInterface
{
public:
	GameClear(const Vector2& position)
		: UserInterface(position)
		, label(std::make_unique<Label>(Filename, FontSize))
	{
		label->Text.str("Stage Clear");
	}

	void Draw() const override
	{
		label->Write(position);
	}

	void Update() override
	{
		
	}
private:
	std::unique_ptr<Label> label;
};

/******************************** UserInterface *********************************/

UserInterface::UserInterface(const Vector2& position)
	: GameObject(true, position)
{}

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
	case UserInterfaceID::GameOver:
		newObject = assignObject<GameOver>(position);
		break;
	case UserInterfaceID::GameClear:
		newObject = assignObject<GameClear>(position);
		break;
	}
	return newObject;
}


std::weak_ptr<IMenu> UserInterfaceManager::GenerateObject(const MenuID id, const Vector2& position)
{
	std::weak_ptr<IMenu> newObject;
	switch (id) {
	case MenuID::Title:
		newObject = assignObject<TitleMenu>(position);
		break;
	}
	return newObject;
}
