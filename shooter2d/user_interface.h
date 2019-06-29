#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "game_object.h"
#include "assets.h"

namespace Shooter {
	class UserInterface : public GameObject
	{
	public:
		UserInterface(const Vector2& positin, std::unique_ptr<Label>&& label);
		virtual ~UserInterface() {}
		virtual void Draw();
		virtual void Update() = 0;
	protected:
		std::unique_ptr<Label> label;
	};

	class UserInterfaceManager : public ObjectManager
	{
	public:
		enum class UserInterfaceID {
			FrameRate,
			GameOver
		};

		std::weak_ptr<UserInterface> GenerateObject(const UserInterfaceID id, const Vector2& position);
	};
}

#endif // !USER_INTERFACE_H
