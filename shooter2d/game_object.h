#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <SDL.h>

const int ScreenWidth = 640;
const int ScreenHeight = 480;

namespace Shooter {
	class GameObject
	{
	public:
		GameObject() = default;
		~GameObject() = default;
		virtual void Draw() = 0;
		virtual void Update() = 0;
	};
}

#endif // !GAME_OBJECT_H