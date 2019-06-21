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

	class GameOverScene : public Scene
	{
	public:
		GameOverScene(IChangingSceneListener& listener);
		void Draw() override;
		void Update() override;
	private:
		std::unique_ptr<UserInterfaceManager> userInterfaceManager;
	};

	class GameScene : public Scene
	{
	public:
		GameScene(IChangingSceneListener& listener);
		void Draw() override;
		void Update() override;
	private:
		std::shared_ptr<PlayerManager> playerManager;
		std::shared_ptr<Player> player;
		std::shared_ptr<UserInterfaceManager> userInterfaceManager;
		std::shared_ptr<EnemyManager> enemyManager;
		std::shared_ptr<BulletManager> bulletManager;
		std::shared_ptr<EffectManager> effectManager;
		std::unique_ptr<CollisionDetector> collisionDetector;
		std::unique_ptr<Script> script;
	};
}

#endif // !SCENE_H
