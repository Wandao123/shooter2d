#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "game_object.h"
#include "assets.h"

namespace Shooter {
	/// <summary>スコアや残機数、メニューの表示などを司る。具体的な挙動は継承先で設定。</summary>
	/// <remarks>継承先のメンバ変数にLabelやSpriteを持たせる場合は、その位置をpositionからの相対位置で設定する。</remarks>
	class UserInterface : public GameObject
	{
	public:
		UserInterface(const Vector2& positin);
		virtual ~UserInterface() {}
	};

	/// <summary>メニューで選択を行うためのインターフェース</summary>
	class IMenu
	{
	public:
		virtual int GetCurrentItemIndex() = 0;
		virtual void Up() = 0;
		virtual void Down() = 0;
	};

	class UserInterfaceManager : public ObjectManager
	{
	public:
		enum class UserInterfaceID {
			FrameRate,
			Title,
			GameOver,
			GameClear
		};
		enum class MenuID {
			Title
		};

		std::weak_ptr<UserInterface> GenerateObject(const UserInterfaceID id, const Vector2& position);
		std::weak_ptr<IMenu> GenerateObject(const MenuID id, const Vector2& position);
	};
}

#endif // !USER_INTERFACE_H
