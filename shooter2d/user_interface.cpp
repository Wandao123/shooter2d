#include <iostream>
#include <iomanip>
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

/******************************** 個別設定用クラス *********************************/

class FrameRate : public UserInterface
{
public:
	FrameRate(const Vector2& position) : UserInterface(position, std::make_unique<Label>(Filename, 14)) {}
private:
	void Update() override
	{
		label->Text.str("");
		label->Text << "FPS " << std::fixed << std::setprecision(3) << Time->GetAverageOfFPS();
	}
};

class GameOver : public UserInterface
{
public:
	GameOver(const Vector2& position) : UserInterface(position, std::make_unique<Label>(Filename, 18)) {}
private:
	void Update() override
	{
		label->Text.str("");
		label->Text << "GAME OVER";
	}
};

/******************************** UserInterface *********************************/

UserInterface::UserInterface(const Vector2& position, std::unique_ptr<Label>&& label)
	: GameObject(true, position)
	, label(std::move(label))
{}

void UserInterface::Draw()
{
	label->Write(position);
}

/******************************** UserInterfaceManager *********************************/

std::weak_ptr<UserInterface> UserInterfaceManager::GenerateObject(const UserInterfaceID id, const Vector2& position)
{
	std::weak_ptr<UserInterface> newObject;
	switch (id) {
	case UserInterfaceID::FrameRate:
		newObject = assignObject<FrameRate>(position);
		break;
	case UserInterfaceID::GameOver:
		newObject = assignObject<GameOver>(position);
		break;
	}
	return newObject;
}
