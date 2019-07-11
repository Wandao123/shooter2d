#ifndef TIMER_H
#define TIMER_H

#include <SDL2/SDL.h>

namespace Shooter {
	// 描画とは独立に動く。
	class Timer
	{
	public:
		static const unsigned int FPS = 60;
		static const unsigned int TicksPerFrame = 1000 / FPS;
		Timer()
			: countedFrames(0), startTicks(0), previousTicks(0), deltaTime(0) {}
		~Timer() = default;
		void Delay();
		void Start();
		void Update();

		float GetDeltaTime()
		{
			return deltaTime;
		}

		unsigned int GetCountedFrames()
		{
			return countedFrames;
		}

		float GetAverageOfFPS()
		{
			return averageOfFPS;
		}

	private:
		unsigned int countedFrames;
		Uint32 startTicks;
		Uint32 previousTicks;
		float deltaTime;  // 前のフレームからの経過時間（秒）
		float averageOfFPS;
	};
}

#endif // !TIMER_H
