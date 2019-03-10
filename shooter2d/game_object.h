#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "vector2.h"

const int ScreenWidth = 640;
const int ScreenHeight = 480;

namespace Shooter {
	class GameObject
	{
	public:
		GameObject() : GameObject({ 0.0f, 0.0f }) {}
		GameObject(const Vector2 &position) : position(position) {}
		~GameObject() = default;
		virtual void Draw() = 0;
		virtual void Update() = 0;

		// HACK: 参照を返したほうがいい？
		Vector2 GetPosition() const
		{
			return position;
		}
	protected:
		Vector2 position;  // ここで言う位置とは画像の左上端のこと。
	};
}

#endif // !GAME_OBJECT_H