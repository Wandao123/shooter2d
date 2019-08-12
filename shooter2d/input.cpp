#include "input.h"
#include <algorithm>

using namespace Shooter;

Input::Input()
{
	for (int i = 0; i < static_cast<int>(Commands::SIZE); i++)
		currentStates[i] = false;

	commandsMapping[Commands::Shot] = SDLK_z;
	commandsMapping[Commands::Bomb] = SDLK_x;
	commandsMapping[Commands::Slow] = SDLK_LSHIFT;
	commandsMapping[Commands::Leftward] = SDLK_LEFT;
	commandsMapping[Commands::Rightward] = SDLK_RIGHT;
	commandsMapping[Commands::Forward] = SDLK_UP;
	commandsMapping[Commands::Backward] = SDLK_DOWN;
	commandsMapping[Commands::Pause] = SDLK_ESCAPE;
}

/// <summary>入力のイベントを検出して、キーおよびボタンの状態を更新する。</summary>
/// <returns>プログラムを終了するならばtrueを返す。</returns>
bool Input::Update()
{
	previousStates = currentStates;
	SDL_Event event;
	downedKeys.clear();
	uppedKeys.clear();

	// キーボードおよびゲームパッドの状態（押されているか離されているか）を取得。
	while (SDL_PollEvent(&event) != 0) {
		switch (event.type) {
		case SDL_QUIT:
			return true;
		case SDL_KEYDOWN:
			/*for (auto mapping : commandsMapping)
				if (event.key.keysym.sym == mapping.second)
					currentStates[static_cast<int>(mapping.first)] = true;*/
			downedKeys.push_front(event.key);
			break;
		case SDL_KEYUP:
			/*for (auto mapping : commandsMapping)
				if (event.key.keysym.sym == mapping.second)
					currentStates[static_cast<int>(mapping.first)] = false;*/
			uppedKeys.push_front(event.key);
			break;
		}
	}

	// キーが押されているか離されているかに応じて、コマンドボタンの状態を更新する。
	for (auto key : downedKeys) {
		auto mapping = std::find_if(commandsMapping.begin(), commandsMapping.end(), [key](const auto& mapObject) -> bool { return mapObject.second == key.keysym.sym; });
		if (mapping != commandsMapping.end())
			currentStates[static_cast<int>(mapping->first)] = true;
	}
	for (auto key : uppedKeys) {
		auto mapping = std::find_if(commandsMapping.begin(), commandsMapping.end(), [key](const auto& mapObject) -> bool { return mapObject.second == key.keysym.sym; });
		if (mapping != commandsMapping.end())
			currentStates[static_cast<int>(mapping->first)] = false;
	}

	return false;
}

/// <summary>現在押されたキーとcommandを対応させる。</summary>
void Input::TranslateInto(const Commands command)
{
	if (!downedKeys.empty())
		commandsMapping[command] = downedKeys.back().keysym.sym;  // 最初に入力したキーに割り当てる。
}

/// <summary>commandに対応するキーが押されているか否か。</summary>
/// <returns>押し続けられていればtrueを返す。</returns>
/// <remarks>押しているかを判定。</remarks>
bool Input::GetKey(const Commands command) const
{
	return currentStates.at(static_cast<int>(command)) ? true : false;
}

/// <summary>commandに対応するキーが押されたか否か。</summary>
/// <returns>押されていればtrueを返す。</returns>
/// <remarks>押した瞬間かを判定。</remarks>
bool Input::GetKeyDown(const Commands command) const
{
	return (!previousStates.at(static_cast<int>(command)) && currentStates.at(static_cast<int>(command))) ? true : false;
}

/// <summary>commandに対応するキーが離されたか否か。</summary>
/// <returns>離されていればtrueを返す。</returns>
/// <remarks>離した瞬間かを判定。</remarks>
bool Input::GetKeyUp(const Commands command) const
{
	return (previousStates.at(static_cast<int>(command)) && !currentStates.at(static_cast<int>(command))) ? true : false;
}

/// <summary>少なくとも一つのキーが押されているか否か。</summary>
/// <returns>押し続けられていればtrueを返す。</returns>
bool Input::IsAnyKeyPressed() const
{
	for (auto key : downedKeys)
		if (key.repeat == 0)
			return true;
	return false;
}

/// <summary>少なくとも一つのキーが押されたか否か。</summary>
/// <returns>押されていればtrueを返す。</returns>
bool Input::IsAnyKeyDown() const
{
	for (auto key : downedKeys)
		if (key.repeat != 0)
			return true;
	return false;
}
