#include <cmath>
#include "timer.h"

using namespace Shooter;

void Timer::Delay()
{
	// 垂直同期をしない場合。小数点以下が桁落ちする分、誤差が生じる。
	int frameTicks = SDL_GetTicks() - previousTicks;
	if (frameTicks < TicksPerFrame)
		SDL_Delay(TicksPerFrame - frameTicks);
}

void Timer::Start()
{
	countedFrames = 0;
	startTicks = SDL_GetTicks();
	previousTicks = startTicks;
}

void Timer::Update()
{
	deltaTime = (countedFrames > 0) ? (SDL_GetTicks() - previousTicks) / 1000.0f : 0.0f;  // Update --> Drow と呼び出される想定。
	averageOfFPS = std::fmod(countedFrames / ((SDL_GetTicks() - startTicks) / 1000.0f), 2000000);
	++countedFrames;
	previousTicks = SDL_GetTicks();  // HACK: いつ更新するかで、Delayを呼び出すタイミングも変わる気がする。現在の実装ではこの関数が真っ先に呼び出されるべき。
}