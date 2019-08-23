#include "timer.h"
#include <algorithm>
#include <cmath>

using namespace Shooter;

void Timer::AdjustFPS(std::function<void(void)> drawingProcess)
{
	if (currentTime < nextFrameTime) {
		drawingProcess();
		if (currentTime < nextFrameTime) {
			SDL_Delay(static_cast<float>(nextFrameTime - currentTime));
			intervalPerFrame = 1000.0f / FPS;
		}
	} else {
		intervalPerFrame *= 2;
	}
	nextFrameTime += intervalPerFrame;
}

void Timer::Start()
{
	countedFrames = 0;
	deltaTime = 0.0f;
	averageOfFPS = 60.0f;
	intervalPerFrame = 1000.0f / FPS;
	currentTime = SDL_GetTicks();
	nextFrameTime = SDL_GetTicks() + intervalPerFrame;
}

void Timer::Update()
{
	++countedFrames;
	Uint32 previousTime = currentTime;
	currentTime = SDL_GetTicks();
	deltaTime = (currentTime - previousTime) / 1000.0f;

	// 平均を求める。
	if (countedFrames % FPS == 0) {
		static Uint32 timeBeforeFPS = 0;
		auto temp = static_cast<float>(currentTime - timeBeforeFPS) / FPS;
		if (temp != 0)
			averageOfFPS = 1000.0f / temp;
		else
			averageOfFPS = 0.0f;
		timeBeforeFPS = currentTime;
	}
}
