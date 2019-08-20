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
		Scene(IChangingSceneListener& listener) : listener(listener) {}
		virtual ~Scene() = default;
		virtual void Draw() const = 0;
		virtual void Update() = 0;
	protected:
		IChangingSceneListener& listener;
	};

	class TitleScene : public Scene
	{
	public:
		TitleScene(IChangingSceneListener& listener);
		void Draw() const override;
		void Update() override;
	private:
		static const int MaxItems = 3;
		const int ItemHeight = 35;  // フォントサイズを考慮。
		std::unique_ptr<UserInterfaceManager> userInterfaceManager;
		int currentIndex;
		std::array<std::weak_ptr<UserInterface>, MaxItems> menu;
	};
}

#endif // !SCENE_H
