#include <iostream>
#include <iomanip>
#include "user_interface.h"

using namespace Shooter;

/******************************** 個別設定用クラス *********************************/

class FrameUI : public UserInterface
{
public:
	FrameUI(const Vector2& position) : UserInterface(position) {}
private:
	void PutText() override
	{
		LoadFont(14);
		Text.str("");
		Text << "FPS " << std::fixed << std::setprecision(3) << Time->GetAverageOfFPS();
	}
};

class GameOver : public UserInterface
{
public:
	GameOver(const Vector2& position) : UserInterface(position) {}
private:
	void PutText() override
	{
		LoadFont(18);
		Text.str("");
		Text << "GAME OVER";
	}
};

/******************************** UserInterface *********************************/

UserInterface::UserInterface(const Vector2& position)
	: GameObject(true, position)
	, Texture(nullptr)
	, Font(nullptr)
	, renderText({ static_cast<int>(position.x), static_cast<int>(position.y), 0, 0 })
{}

UserInterface::~UserInterface()
{
	if (Font != nullptr) {
		TTF_CloseFont(Font);
		Font = nullptr;
	}
}

void UserInterface::Draw()
{
	SDL_RenderCopy(Renderer, Texture, nullptr, &renderText);
}

void UserInterface::Update()
{
	PutText();
	int textWidth = 0, textHeight = 0;
	SDL_Color textColor = { 0xFF, 0xFF, 0xFF };
	SDL_Surface *textSurface = TTF_RenderText_Solid(Font, Text.str().c_str(), textColor);
	Texture = SDL_CreateTextureFromSurface(Renderer, textSurface);
	textWidth = textSurface->w;
	textHeight = textSurface->h;
	SDL_FreeSurface(textSurface);
	renderText = { static_cast<int>(position.x), static_cast<int>(position.y), textWidth, textHeight };
}

void UserInterface::LoadFont(const unsigned int size)
{
	if (Font != nullptr) {
		TTF_CloseFont(Font);
		Font = nullptr;
	}
	// TODO: 例外の発生。
#ifdef _WIN64
	Font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", size);
#elif __linux__
	Font = TTF_OpenFont("/usr/share/fonts/TTF/LiberationSans-Regular.ttf", size);
#elif __unix__
	// Monospace font を指定
#endif
	if (Font == nullptr) {
		std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
	}
}

/******************************** UserInterfaceManager *********************************/

std::weak_ptr<UserInterface> UserInterfaceManager::GenerateObject(const UserInterfaceID id, const Vector2& position)
{
	std::weak_ptr<UserInterface> newObject;
	switch (id) {
	case UserInterfaceID::FrameRate:
		newObject = assignObject<FrameUI>(position);
		break;
	case UserInterfaceID::GameOver:
		newObject = assignObject<GameOver>(position);
		break;
	}
	return newObject;
}
