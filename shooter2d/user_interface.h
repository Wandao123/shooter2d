﻿#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <SDL_ttf.h>
#include "game_object.h"

namespace Shooter {
	extern SDL_Renderer *Renderer;
	extern TTF_Font *Font;

	class UserInterface : public GameObject
	{
	public:
		UserInterface(const Vector2& positin);
		virtual ~UserInterface();
		virtual void Draw();
		virtual void Update();
	protected:
		SDL_Texture *Texture;
		TTF_Font *Font;
		std::stringstream Text;
		virtual void PutText() = 0;
	private:
		SDL_Rect renderText;
	};

	class FrameUI : public UserInterface
	{
	public:
		FrameUI(const Vector2& position) : UserInterface(position) {}
		~FrameUI() = default;
	private:
		void PutText() override;
	};

	class UserInterfaceManager : public ObjectManager
	{
	public:
		enum class UserInterfaceID {
			FrameRate
		};

		std::shared_ptr<UserInterface> GenerateObject(const UserInterfaceID id, const Vector2& position);
	};
}

#endif // !USER_INTERFACE_H