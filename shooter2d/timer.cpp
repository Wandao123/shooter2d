#include <cmath>
#include "timer.h"

using namespace Shooter;

void Timer::Delay()
{
	// �������������Ȃ��ꍇ�B�����_�ȉ������������镪�A�덷��������B
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
	deltaTime = (countedFrames > 0) ? (SDL_GetTicks() - previousTicks) / 1000.0f : 0.0f;  // Update --> Drow �ƌĂяo�����z��B
	averageOfFPS = std::fmod(countedFrames / ((SDL_GetTicks() - startTicks) / 1000.0f), 2000000);
	++countedFrames;
	previousTicks = SDL_GetTicks();  // HACK: ���X�V���邩�ŁADelay���Ăяo���^�C�~���O���ς��C������B���݂̎����ł͂��̊֐����^����ɌĂяo�����ׂ��B
}