#include "input.h"
#include <algorithm>
#include <iostream>

using namespace Shooter;

/*// setを使うための順序の定義。
bool operator<(const SDL_KeyboardEvent& lhs, const SDL_KeyboardEvent& rhs)
{
	return lhs.keysym.sym < rhs.keysym.sym;
}

// setを使うための順序の定義。
bool operator<(const SDL_ControllerButtonEvent& lhs, const SDL_ControllerButtonEvent& rhs)
{
	return lhs.button < rhs.button;
}*/

Input::Input()
{
	// ゲームパッドの初期化。
	if (SDL_NumJoysticks() > 0) {
		if (SDL_IsGameController(0))
			gameController = SDL_GameControllerOpen(0);
		if (gameController == nullptr)
			std::cout << "Warning: Unable to open game controller! SDL Error: " << SDL_GetError() << std::endl;
	}
	SDL_GameControllerEventState(SDL_ENABLE);

	// デフォルトの値を設定。
	commandsMapping[Commands::Shot] = std::make_tuple(SDLK_z, SDL_CONTROLLER_BUTTON_A);
	commandsMapping[Commands::Bomb] = std::make_tuple(SDLK_x, SDL_CONTROLLER_BUTTON_B);
	commandsMapping[Commands::Slow] = std::make_tuple(SDLK_LSHIFT, SDL_CONTROLLER_BUTTON_X);
	commandsMapping[Commands::Skip] = std::make_tuple(SDLK_LCTRL, SDL_CONTROLLER_BUTTON_Y);
	commandsMapping[Commands::Leftward] = std::make_tuple(SDLK_LEFT, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	commandsMapping[Commands::Rightward] = std::make_tuple(SDLK_RIGHT, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	commandsMapping[Commands::Forward] = std::make_tuple(SDLK_UP, SDL_CONTROLLER_BUTTON_DPAD_UP);
	commandsMapping[Commands::Backward] = std::make_tuple(SDLK_DOWN, SDL_CONTROLLER_BUTTON_DPAD_DOWN);
	commandsMapping[Commands::Pause] = std::make_tuple(SDLK_ESCAPE, SDL_CONTROLLER_BUTTON_START);
	for (int i = 0; i < static_cast<int>(Commands::SIZE); i++)
		currentStates[i] = false;
}

Input::~Input()
{
	SDL_GameControllerClose(gameController);
	gameController = nullptr;
}

/// <summary>入力のイベントを検出して、キーおよびボタンの状態を更新する。</summary>
/// <returns>プログラムを終了するならばtrueを返す。</returns>
bool Input::Update()
{
	previousStates = currentStates;
	SDL_Event event;
	downedKeys.clear();
	uppedKeys.clear();
	downedButtons.clear();
	uppedButtons.clear();

	// キーボードおよびゲームパッドの状態（押されているか離されているか）を取得。
	while (SDL_PollEvent(&event) != 0) {
		switch (event.type) {
		case SDL_QUIT:
			return true;
		case SDL_KEYDOWN:
			downedKeys.push_back(event.key);
			break;
		case SDL_KEYUP:
			uppedKeys.push_back(event.key);
			break;
		case SDL_CONTROLLERBUTTONDOWN:
			downedButtons.push_back(event.cbutton);
			break;
		case SDL_CONTROLLERBUTTONUP:
			uppedButtons.push_back(event.cbutton);
			break;
		}
	}

	// キーが押されているか離されているかに応じて、コマンドボタンの状態を更新する。
	for (auto key : downedKeys) {
		auto mapping = std::find_if(commandsMapping.begin(), commandsMapping.end(), [key](const auto& mapObject) -> bool { return std::get<0>(mapObject.second) == key.keysym.sym; });
		if (mapping != commandsMapping.end())
			currentStates[static_cast<int>(mapping->first)] = true;
	}
	for (auto key : uppedKeys) {
		auto mapping = std::find_if(commandsMapping.begin(), commandsMapping.end(), [key](const auto& mapObject) -> bool { return std::get<0>(mapObject.second) == key.keysym.sym; });
		if (mapping != commandsMapping.end())
			currentStates[static_cast<int>(mapping->first)] = false;
	}

	// コントローラボタンが押されているか離されているかに応じて、コマンドボタンの状態を更新する。
	for (auto button : downedButtons) {
		//std::cout << SDL_GameControllerGetStringForButton(static_cast<SDL_GameControllerButton>(button.button)) << std::endl;
		auto mapping = std::find_if(commandsMapping.begin(), commandsMapping.end(), [button](const auto& mapObject) -> bool { return std::get<1>(mapObject.second) == static_cast<SDL_GameControllerButton>(button.button); });
		if (mapping != commandsMapping.end())
			currentStates[static_cast<int>(mapping->first)] = true;
	}
	for (auto button : uppedButtons) {
		auto mapping = std::find_if(commandsMapping.begin(), commandsMapping.end(), [button](const auto& mapObject) -> bool { return std::get<1>(mapObject.second) == static_cast<SDL_GameControllerButton>(button.button); });
		if (mapping != commandsMapping.end())
			currentStates[static_cast<int>(mapping->first)] = false;
	}

	return false;
}

/// <summary>現在押されたキーとcommandを対応させる。</summary>
void Input::TranslateKeyInto(const Commands command)
{
	if (!downedKeys.empty())
		std::get<0>(commandsMapping[command]) = downedKeys.begin()->keysym.sym;
}

/// <summary>現在押されたボタンとcommandを対応させる。</summary>
void Input::TranslateButtonInto(const Commands command)
{
	if (!downedButtons.empty())
		std::get<1>(commandsMapping[command]) = static_cast<SDL_GameControllerButton>(downedButtons.begin()->button);
}

/// <summary>commandに対応するキー・ボタンが押されているか否か。</summary>
/// <returns>押し続けられていればtrueを返す。</returns>
/// <remarks>押しているかを判定。</remarks>
bool Input::GetKey(const Commands command) const
{
	return currentStates.at(static_cast<int>(command)) ? true : false;
}

/// <summary>commandに対応するキー・ボタンが押されたか否か。</summary>
/// <returns>押されていればtrueを返す。</returns>
/// <remarks>押した瞬間かを判定。</remarks>
bool Input::GetKeyDown(const Commands command) const
{
	return (!previousStates.at(static_cast<int>(command)) && currentStates.at(static_cast<int>(command))) ? true : false;
}

/// <summary>commandに対応するキー・ボタンが離されたか否か。</summary>
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

/// <summary>少なくとも一つのボタンが押されているか否か。</summary>
/// <returns>押されていればtrueを返す。</returns>
bool Input::IsAnyButtonPressed() const
{
	for (auto button : downedButtons)
		if (button.state == SDL_PRESSED)
			return true;
	return false;
}