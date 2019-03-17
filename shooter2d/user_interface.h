#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <SDL_ttf.h>
#include "game_object.h"

namespace Shooter {
	extern SDL_Renderer *Renderer;
	extern TTF_Font *Font;

	class UserInterface : public GameObject
	{
	public:
		UserInterface(const int positionX, const int positionY);
		~UserInterface();
		virtual void Draw();
		virtual void Update();
	protected:
		SDL_Texture *Texture;
		TTF_Font *Font;
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
		void PutText() override;
	};
}

#endif // !USER_INTERFACE_H