#include <iostream>
#include <iomanip>
#include "user_interface.h"

using namespace Shooter;

UserInterface::UserInterface(const Vector2& position)
	: GameObject(true, position)
{
	// TODO: 例外の発生。
#ifdef _WIN64
	Font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 14);
//#elif __linux__
#else
	Font = TTF_OpenFont("/usr/share/fonts/TTF/LiberationSans-Regular.ttf", 14);
#endif
	if (Font == nullptr) {
		std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
	}
}

UserInterface::~UserInterface()
{
	TTF_CloseFont(Font);
	Font = nullptr;
	TTF_Quit();
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
	//renderText = { 0, ScreenHeight - textHeight, textWidth, textHeight };
	renderText = { static_cast<int>(position.x), static_cast<int>(position.y), textWidth, textHeight };
}

void FrameUI::PutText()
{
	Text.str("");
	Text << "FPS " << std::fixed << std::setprecision(3) << Time->GetAverageOfFPS();
}

std::shared_ptr<UserInterface> UserInterfaceManager::GenerateObject(const UserInterfaceID id, const Vector2& position)
{
	std::shared_ptr<UserInterface> newObject;
	switch (id) {
	case UserInterfaceID::FrameRate:
		newObject = assignObject<FrameUI>(position);
		break;
	}
	return newObject;
}