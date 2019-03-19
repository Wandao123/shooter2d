#ifndef SCENE_H
#define SCENE_H

#include <list>
#include <lua.hpp>
#include "game_object.h"

namespace Shooter {
	class Scene
	{
	public:
		Scene() = default;
		virtual ~Scene() = default;
		virtual void Draw() = 0;
		virtual void Update() = 0;
	};

	class GameScene : public Scene
	{
	public:
		GameScene();
		~GameScene();
		void Draw() override;
		void Update() override;
	private:
		/*struct luaStateCloser {
			void operator()(lua_State* l) const
			{
				lua_close(l);
			}
		};*/
		//std::function<void(lua_State*)> luaStateCloser = [](lua_State* l) { lua_close(l); };

		std::list<std::shared_ptr<GameObject>> tasksList;
		//lua_State* state;  // HACK: スマートポインタを使いたかったが、カスタムデリータの設定が巧くゆかなかった。
		std::unique_ptr<lua_State, std::function<void(lua_State*)>> state;
		void run();
	};
}

#endif // !SCENE_H
