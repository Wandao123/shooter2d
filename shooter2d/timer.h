#ifndef TIMER_H
#define TIMER_H

#include <sstream>
#include <SDL.h>

namespace Shooter {
	// �`��Ƃ͓Ɨ��ɓ����B
	class Timer
	{
	public:
		static const int FPS = 60;
		static const int TicksPerFrame = 1000 / FPS;
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

		unsigned GetCountedFrames()
		{
			return countedFrames;
		}

		float GetAverageOfFPS()
		{
			return averageOfFPS;
		}

	private:
		unsigned countedFrames;
		Uint32 startTicks;
		Uint32 previousTicks;
		float deltaTime;  // �O�̃t���[������̌o�ߎ��ԁi�b�j
		float averageOfFPS;
	};
}

#endif // !TIMER_H
