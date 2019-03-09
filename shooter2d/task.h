#ifndef TASK_H
#define TASK_H

#include <SDL.h>

const int ScreenWidth = 640;
const int ScreenHeight = 480;

namespace Shooter {
	// 処理すべき対象。
	class Task
	{
	public:
		Task() = default;
		~Task() = default;
		virtual void Draw() = 0;
		virtual void Update() = 0;
	};
}

#endif // !TASK_H