#ifndef SCRIPT_H
#define SCRIPT_H

#include <sol.hpp>
#include <functional>
#include "mover.h"

namespace Shooter {
	class GameScene;

	class Script
	{
	public:
		Script(const GameScene& gameScene);
		~Script();
		void Run();
	private:
		const GameScene& gameScene;
		sol::state lua;
		// HACK: solのコルーチンの呼び出し方のために、組で保持する必要がある。Lua側でcoroutine.createをすれば、スレッドだけでもよい？
		std::list<std::pair<sol::thread, sol::coroutine>> tasksList;  // これに登録されたコルーチンが毎フレーム実行される。

		// 敵の生成。種類が増えたときに面倒なので、Lua側ではなくC++側で生成する。
		std::shared_ptr<Enemy> generateEnemy(const EnemyManager::EnemyID id, const float posX, const float posY, const float speed, const float angle, const int hitPoint);

		// 指定された位置に敵弾を生成する。生成元の存在は問わない。
		std::shared_ptr<Bullet> generateBullet(const BulletManager::BulletID id, const float posX, const float posY, const float speed, const float angle);

		// 指定した敵から敵弾を生成する。生成元が存在しなければ生成しない。
		std::shared_ptr<Bullet> generateBulletFromEnemy(const BulletManager::BulletID id, std::shared_ptr<Enemy> enemy, const float speed, const float angle);

		// 本来はコルーチンを直接受け取って実行したい。しかし、スレッドを生成した上で呼び出すには、文字列で関数名を受け取る必要がある。
		void startCoroutine(const std::string name);

		void startCoroutineWithArgs(const std::string name, sol::variadic_args va);
	};
}

#endif // !SCRIPT_H