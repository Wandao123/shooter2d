#ifndef GAME_H
#define GAME_H

#include <memory>
#include <stack>
#include "timer.h"
#include "scene.h"

namespace Shooter {
	class Game
	{
	public:
		static const int Width = 640;
		static const int Height = 480;
		Game();
		~Game();
		void Run();
	private:
		//void changeScene(Scene newScene);
		std::stack<std::unique_ptr<Scene>> scenes;
	};
}

#endif // !GAME_H
