#ifndef SCENE_H
#define SCENE_H

#include <list>
#include "game_object.h"

namespace Shooter {
	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;
		virtual void Draw() = 0;
		virtual void Update() = 0;
	};

	class GameScene : public Scene
	{
	public:
		GameScene();
		void Draw() override;
		void Update() override;
	private:
		std::list<std::shared_ptr<GameObject>> tasksList;
		void run();
	};
}

#endif // !SCENE_H
