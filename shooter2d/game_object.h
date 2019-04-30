#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <list>
#include <memory>
#include "vector2.h"
#include "timer.h"

namespace Shooter {
	extern std::unique_ptr<Timer> Time;

	class GameObject
	{
	public:
		GameObject() : GameObject({ 0.0f, 0.0f }) {}
		GameObject(const Vector2 position) : enabled(true), position(position) {}
		~GameObject() = default;
		virtual void Draw() = 0;
		virtual void Update() = 0;

		// HACK: 参照を返したほうがいい？
		Vector2 GetPosition() const
		{
			return position;
		}

		bool IsEnabled() const
		{
			return enabled;
		}
	protected:
		bool enabled;
		Vector2 position;  // ここで言う位置とは画像の中心のこと。
	};

	class ObjectManager
	{
	public:
		ObjectManager() {}
		virtual ~ObjectManager() {}
		virtual void Draw();
		// 本来は抽象メンバ関数を定義して、継承先に強制実装させたいが、生成されるものによって引数の数が異なる。
		// もし生成処理を共通化したいならば、後からメンバ変数を初期化するためのメンバ関数を定義する。
		//virtual void GenerateObject() = 0;
		virtual void Update();
	protected:
		std::list<std::shared_ptr<GameObject>> objectsList;  // 更新対象オブジェクト。
	};
}

#endif // !GAME_OBJECT_H
