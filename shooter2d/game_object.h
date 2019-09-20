#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <list>
#include <memory>
#include "vector2.h"
#include "timer.h"

namespace Shooter {
	/// <summary>ゲーム中に表示される全てのオブジェクトの親クラス。</summary>
	/// <remarks>各最終継承先にはpositionのみを引数に持つコンストラクタを定義せよ。</remarks>
	class GameObject
	{
	public:
		GameObject(const bool enabled, const Vector2 position) : enabled(enabled), position(position) {}
		virtual ~GameObject() = default;
		virtual void Draw() const = 0;
		virtual void Update() = 0;

		// HACK: 参照を返したほうがいい？
		Vector2 GetPosition() const
		{
			return position;
		}

		void SetPosition(const Vector2& position)  // HACK: 右辺値参照と左辺値参照とを使い分けるべき。
		{
			this->position = position;
		}

		void Erase()
		{
			enabled = false;
		}

		bool IsEnabled() const
		{
			return enabled;
		}
	protected:
		bool enabled;
		Vector2 position;  // ここで言う位置とは画像の中心のこと。
	};

	/// <summary>GameObjectの管理クラス。</summary>
	/// <remarks>
	/// 生成するオブジェクトの種類に応じて、このクラスの継承先で定義せよ。特に、インスタンスを返す関数を定義していないので、
	///     std::weak_ptr GenerateObject(生成するオブジェクトのID, 生成する位置)
	/// という型の関数を継承先で定義すること。下記のassignObjectも参照。
	/// </remarks>
	class ObjectManager
	{
	public:
		ObjectManager() {}
		virtual ~ObjectManager() {}
		virtual void Draw() const;
		virtual void Update();

		std::list<std::weak_ptr<GameObject>> GetList() const
		{
			std::list<std::weak_ptr<GameObject>> temp;
			for (auto&& object : objectsList)
				if (object->IsEnabled())
					temp.push_back(object);
			return temp;
		}
	protected:
		std::list<std::shared_ptr<GameObject>> objectsList;  // 更新対象オブジェクト。本来なら継承先でGameObjectの具象クラスに対して定義するべきか？

		/// <summary>未使用のObjectType型のオブジェクトをobjectsListから探索して、見つかればそれを返す。見つからなければ新しく生成する。</summary>
		/// <remarks>
		/// ObjectTypeはGameObjectの子クラスであることと、そのコンストラクタの引数はpositionのみであることが前提。
		/// また、返り値を受け取ったらすぐに「実体化関数」を呼ぶかlockを呼び出すこと。さもなければ、この関数はweak_ptrを返す
		/// （参照カウントが増えない）ので、未使用と見なされる。
		/// </remarks>
		template <class ObjectType>
		std::weak_ptr<ObjectType> assignObject(const Vector2& position)
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
