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
		virtual void Draw() = 0;
		virtual void Update() = 0;
	protected:
		IChangingSceneListener& listener;
	};

	class TitleScene : public Scene
	{
	public:
		TitleScene(IChangingSceneListener& listener);
		void Draw() override;
		void Update() override;
	private:
		std::unique_ptr<UserInterfaceManager> userInterfaceManager;
		std::weak_ptr<IMenu> menu;
	};
}

#endif // !SCENE_H
