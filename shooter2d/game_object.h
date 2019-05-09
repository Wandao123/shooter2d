#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <list>
#include <memory>
#include "vector2.h"
#include "timer.h"

namespace Shooter {
	extern std::unique_ptr<Timer> Time;

	// ゲーム中に表示される全てのオブジェクトの親クラス。各最終継承先にはpositionのみを引数に持つコンストラクタを定義せよ（仕様）。
	class GameObject
	{
	public:
		GameObject() : GameObject(true, { 0.0f, 0.0f }) {}
		GameObject(const bool enabled, const Vector2 position) : enabled(enabled), position(position) {}
		~GameObject() = default;
		virtual void Draw() = 0;
		virtual void Update() = 0;

		// HACK: 参照を返したほうがいい？
		Vector2 GetPosition() const
		{
			return position;
		}

		void SetPosition(const Vector2& position)
		{
			this->position = position;
		}

		bool IsEnabled() const
		{
			return enabled;
		}
	protected:
		bool enabled;
		Vector2 position;  // ここで言う位置とは画像の中心のこと。
	};

	// GameObjectの管理クラス。ここでは生成処理が定義されていない。生成するオブジェクトの種類に応じて、このクラスの継承先で定義せよ。
	class ObjectManager
	{
	public:
		ObjectManager() {}
		virtual ~ObjectManager() {}
		virtual void Draw();
		virtual void Update();

		std::list<std::shared_ptr<GameObject>> GetList() const
		{
			std::list<std::shared_ptr<GameObject>> temp;
			for (auto&& object : objectsList)
				if (object->IsEnabled())
					temp.push_back(object);
			return temp;
		}
	protected:
		std::list<std::shared_ptr<GameObject>> objectsList;  // 更新対象オブジェクト。本来なら継承先でGameObjectの具象クラスに対して定義するべきか？

		// 未使用のObjectType型のオブジェクトをobjectsListから探索して、見つかればそれを返す。見つからなければ新しく生成する。
		// ObjectTypeはGameObjectの子クラスであることと、そのコンストラクタの引数はpositionのみであることが前提。
		template <class ObjectType>
		std::shared_ptr<ObjectType> assignObject(const Vector2& position)
		{
			for (auto&& object : objectsList)
				if (typeid(*object) == typeid(ObjectType) && !object->IsEnabled() && object.use_count() <= 1) {
					object->SetPosition(position);
					return std::dynamic_pointer_cast<ObjectType>(object);
				}
			objectsList.push_front(std::make_unique<ObjectType>(position));
			return std::dynamic_pointer_cast<ObjectType>(*objectsList.begin());;
		}
	};
}

#endif // !GAME_OBJECT_H
