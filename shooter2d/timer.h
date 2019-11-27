#ifndef TIMER_H
#define TIMER_H

#include <functional>
//#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "singleton.h"

namespace Shooter {
	/// <summary>FPS管理やフレーム数の数え上げを司るクラス。</summary>
	class Timer : public Singleton<Timer>
	{
		Timer();
		~Timer() = default;
		friend class Singleton<Timer>;
	public:
		static const unsigned int FPS = 60;
		void AdjustFPS(std::function<void(void)> drawingProcess);
		void Start();
		void Stop();
		void Restart();
		void Update();

		/// <summary>直前のフレームからの経過時間を取得する。</summary>
		double GetDeltaTime()
		{
			return deltaTime;
		}

		/// <summary>プレイしたフレーム数を取得する。</summary>
		/// <remarks>停止しているフレーム数だけ補正する。</remarks>
		unsigned int GetPlayingFrames()
		{
			return countedFrames - stoppingFrames;
		}

		/// <summary>FPS定数のフレーム数だけ計測した、FPSの平均値を取得する。</summary>
		double GetAverageOfFPS()
		{
			return averageOfFPS;
		}
	private:
		Uint32 countedFrames;
		double deltaTime;        // 前のフレームからの経過時間（秒）。
		double averageOfFPS;
		double intervalPerFrame;
		Uint32 currentTime;     // Startメンバ関数が呼ばれてからの経過フレーム数。
		double nextFrameTime;
		bool isStopping;        // 停止するためのフラグ。
		Uint32 stoppingFrames;  // 停止したフレーム数。
	};
}

#endif // !TIMER_H
