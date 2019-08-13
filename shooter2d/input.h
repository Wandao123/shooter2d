﻿#ifndef INPUT_H
#define INPUT_H

#include "singleton.h"
#include <array>
//#include <set>
#include <list>
#include <map>
#include <tuple>
#include <SDL2/SDL.h>

namespace Shooter {
	/// <summary>入力を管理するクラス。キーボードの入力とゲームパッドのそれとを統合する。</summary>
	class Input final : public Singleton<Input>
	{
		Input();
		~Input();
		friend class Singleton<Input>;
	public:
		enum class Commands : int {  // ゲームで使用するコマンド。
			Shot,
			Bomb,
			Slow,
			Skip,
			Leftward,
			Rightward,
			Forward,
			Backward,
			Pause,
			SIZE  // 要素数を取得するためのダミー。
		};

		bool Update();
		void TranslateKeyInto(const Commands command);
		void TranslateButtonInto(const Commands command);
		bool GetKey(const Commands command) const;
		bool GetKeyDown(const Commands command) const;
		bool GetKeyUp(const Commands command) const;
		bool IsAnyKeyPressed() const;
		bool IsAnyKeyDown() const;
		bool IsAnyButtonPressed() const;
	private:
		std::map<Commands, std::tuple<SDL_Keycode, SDL_GameControllerButton>> commandsMapping;
		// enum classからintへの暗黙的な型変換が無いため、明示的にキャストする必要がある。
		std::array<bool, static_cast<int>(Commands::SIZE)> previousStates, currentStates;  // 1フレーム前と現在のコマンドボタンの状態。
		std::list<SDL_KeyboardEvent> downedKeys, uppedKeys;  // 1フレームで押された・離されたキーの種類。
		SDL_GameController* gameController = nullptr;
		std::list<SDL_ControllerButtonEvent> downedButtons, uppedButtons;  // 1フレームで押された・離されたコントローラボタンの種類。
	};
}

#endif // !INPUT_H
