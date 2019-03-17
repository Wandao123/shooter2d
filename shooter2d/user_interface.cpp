#include <iostream>
#include <iomanip>
#include "user_interface.h"

using namespace Shooter;

UserInterface::UserInterface(const int positionX, const int positionY)
	: posX(positionX)
	, posY(positionY)
{
	// TODO: 例外の発生。
	Font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 14);
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
	renderText = { posX, posY, textWidth, textHeight };
}

void FrameUI::PutText()
{
	Text.str("");
	Text << "FPS " << std::fixed << std::setprecision(3) << Time->GetAverageOfFPS();
}