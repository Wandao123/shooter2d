#ifndef SCENE_H
#define SCENE_H

#include <utility>
#include "collision_detector.h"
#include "user_interface.h"
#include "script.h"

namespace Shooter {
	class IChangingSceneListener;

	class Scene
	{
	public:
		enum class ParameterKeys {
			Character,
			Level,
			Stage
		};
		using Parameters = std::map<ParameterKeys, int>;  // 型安全にするならstructの方が良い？

		Scene(IChangingSceneListener& listener, Parameters& parameters)
			: listener(listener)
			, parameters(parameters)
			, previousPressedFrame(Timer::Create().GetPlayingFrames())
			, counter(0)
		{}
		virtual ~Scene() = default;
		virtual void Draw() const = 0;
		virtual void Update() = 0;
	protected:
		IChangingSceneListener& listener;
		Parameters& parameters;
		void adjustWithKeys(Input::Commands decrement, std::function<void(void)> decrease, Input::Commands increment, std::function<void(void)> increase);
		void waitAndDo(const unsigned int delayFrames, std::function<void(void)> func);
	private:
		unsigned int previousPressedFrame;  // 直前のキーが押されたフレーム。
		unsigned int counter;               // waitAndDoで使うためのカウンタ。
	};

	class TitleScene : public Scene
	{
	public:
		TitleScene(IChangingSceneListener& listener, Parameters& parameters);
		void Draw() const override;
		void Update() override;
	private:
		static const int MaxItems = 3;
		const int ItemHeight = UserInterfaceManager::FontSize * 5 / 4;
		std::unique_ptr<UserInterfaceManager> userInterfaceManager;
		int currentIndex;
		std::array<std::weak_ptr<UserInterface>, MaxItems> menu;
	};
}

#endif // !SCENE_H
