#include "timer.h"
#include <algorithm>
#include <cmath>

using namespace Shooter;

/// <summary>垂直同期をしない場合に必要な時間だけ待機する。小数点以下が桁落ちする分、誤差が生じる。</summary>
void Timer::Delay()
{
	auto delayingTime = [this]() -> Uint32 {
		if (countedFrames <= 1) {
			return 0;
		} else {
			int idealTime = static_cast<int>(static_cast<int>(ticksList.size()) * 1000.0f / FPS);
			int realTime = SDL_GetTicks() - ticksList.front();
			int temp = idealTime - realTime;
			return static_cast<Uint32>(std::max(temp, 0));
		}
	};

	SDL_Delay(delayingTime());
}

void Timer::Start()
{
	countedFrames = 0;
	deltaTime = 0.0f;
	averageOfFPS = 60.0f;
	ticksList.clear();
}

void Timer::Update()
{
	++countedFrames;
	ticksList.push_back(SDL_GetTicks());
	if (ticksList.size() > MaxStoredTicks)
		ticksList.pop_front();

	// 平均を求める。
	if (countedFrames % UpdatingInterval == 0) {
		auto temp = static_cast<float>(ticksList.back() - ticksList.front()) / static_cast<int>(ticksList.size() - 1);
		if (temp != 0)
			averageOfFPS = 1000.0f / temp;
		else
			averageOfFPS = 0.0f;
	}

	// 直前のフレームからの経過時間。
	if (countedFrames <= 1) {
		return;
	} else {
		auto itr = ticksList.end();
		--itr;  // 現在の時刻。
		--itr;  // 1フレーム前の時刻。
		deltaTime = (countedFrames > 0) ? (ticksList.back() - *itr) / 1000.0f : 0.0f;  // Update --> Drow と呼び出される想定。
	}
}
