#include "timer.h"
#include <algorithm>
#include <cmath>

using namespace Shooter;


Timer::Timer()
	: countedFrames(0)
	, deltaTime(0)
	, averageOfFPS(60)
	, intervalPerFrame(1000.0 / FPS)
	, currentTime(SDL_GetTicks())
	, nextFrameTime(SDL_GetTicks() + intervalPerFrame)
	, isStopping(true)
	, stoppingFrames(0)
{}

void Timer::AdjustFPS(std::function<void(void)> drawingProcess)
{
	if (currentTime < nextFrameTime) {
		drawingProcess();
		if (currentTime < nextFrameTime) {
			SDL_Delay(static_cast<Uint32>(nextFrameTime - currentTime));
			intervalPerFrame = 1000.0 / FPS;
		}
	} else {
		intervalPerFrame *= 2;
	}
	nextFrameTime += intervalPerFrame;
}

void Timer::Start()
{
	countedFrames = 0;
	deltaTime = 0.0;
	averageOfFPS = 60.0;
	intervalPerFrame = 1000.0 / FPS;
	currentTime = SDL_GetTicks();
	nextFrameTime = SDL_GetTicks() + intervalPerFrame;
	isStopping = false;
	stoppingFrames = 0;
}

void Timer::Stop()
{
	isStopping = true;
}

void Timer::Restart()
{
	isStopping = false;
}

void Timer::Update()
{
	Uint32 previousTime = currentTime;
	currentTime = SDL_GetTicks();
	deltaTime = static_cast<double>(currentTime - previousTime) / 1000.0;

	if (isStopping)
		++stoppingFrames;
	++countedFrames;
	if (countedFrames % FPS == 0) {
		static Uint32 timeBeforeFPS = 0;
		auto temp = static_cast<double>(currentTime - timeBeforeFPS) / FPS;
		if (temp != 0)
			averageOfFPS = 1000.0 / temp;
		else
			averageOfFPS = 0.0;
		timeBeforeFPS = currentTime;
		}
}
