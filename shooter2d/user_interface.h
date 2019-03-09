#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <iostream>
#include <sstream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "game_object.h"
#include "timer.h"

namespace Shooter {
	extern SDL_Window *Window;
	extern SDL_Renderer *Renderer;
	extern TTF_Font *Font;
	extern std::unique_ptr<Timer> Time;

	class UserInterface : public GameObject
	{
	public:
		UserInterface(const int positionX, const int positionY)
			: posX(positionX)
			, posY(positionY)
		{
			// TODO: 例外の発生。
			/*Font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 14);
			if (Font == nullptr) {
				std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
			}*/
		}

		~UserInterface()
		{
			/*std::cout << Font << " " << (Font == nullptr) << std::endl;
			TTF_CloseFont(Font);
			Font = nullptr;*/
		}

		virtual void Draw()
		{
			SDL_RenderCopy(Renderer, Texture, nullptr, &renderText);
		}

		virtual void Update()
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

	protected:
		SDL_Texture *Texture;
		//TTF_Font *Font;
		std::stringstream Text;
		virtual void PutText() = 0;
	private:
		SDL_Rect renderText;
		int posX, posY;
	};

	class FrameUI : public UserInterface
	{
	public:
		FrameUI(const int positionX, const int positionY) : UserInterface(positionX, positionY) {}
		~FrameUI() = default;
	private:
		void PutText();
	};
}

#endif // !USER_INTERFACE_H