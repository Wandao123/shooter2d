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
		virtual void Update() = 0;
		virtual void Draw() = 0;
	};

	class GameScene : public Scene
	{
	public:
		GameScene();
		void Update() override;
		void Draw() override;
	private:
		std::list<std::unique_ptr<GameObject>> tasksList;
	};
}

#endif // !SCENE_H
