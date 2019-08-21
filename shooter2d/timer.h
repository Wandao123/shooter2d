#ifndef TIMER_H
#define TIMER_H

#include <list>
//#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "singleton.h"

namespace Shooter {
	/// <summary>FPS管理やフレーム数を共通化するクラス。他のゲーム用のオブジェクトとは独立に扱う。</summary>
	class Timer : public Singleton<Timer>
	{
		Timer() : countedFrames(0), deltaTime(0), averageOfFPS(60) {}
		~Timer() = default;
		friend class Singleton<Timer>;
	public:
		static const unsigned int FPS = 60;
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
		const std::size_t MaxStoredTicks = 120;
		const unsigned int UpdatingInterval = 60;

		unsigned int countedFrames;
		float deltaTime;  // 前のフレームからの経過時間（秒）
		float averageOfFPS;
		std::list<Uint32> ticksList;
	};
}

#endif // !TIMER_H
