#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "game_object.h"
#include "assets.h"
#include "input.h"

namespace Shooter {
	/// <summary>スコアや残機数、メニューのボタン、値の調節などを司る。具体的な挙動は継承先で設定。</summary>
	/// <remarks>規約：継承先のメンバ変数にLabelやSpriteを持たせる場合は、その位置をpositionからの相対位置で設定する。</remarks>
	class UserInterface : public GameObject
	{
	public:
		UserInterface(const Vector2& positin);
		virtual ~UserInterface() {}
		virtual void Update() override;
		virtual void OnKeyPressed(Input::Commands command) {}  // commandボタンが押された時のイベントハンドラ。
		virtual void OnKeyPressed() {}                         // 何れかのキー・ボタンが押された時のイベントハンドラ。

		void SetCaption(std::string caption)
		{
			this->caption = caption;
		}

		void SetActive(bool activated)
		{
			this->activated = activated;
		}
	protected:
		std::string caption;
		bool activated;
	};

	/// <summary>ゲームの状態を表示するUIのためのインターフェース。</summary>
	class IStatusMonitor
	{
	public:
		IStatusMonitor() = default;
		virtual ~IStatusMonitor() = default;

		virtual void SetValue(unsigned int value)
		{
			this->value = value;
		}
	protected:
		unsigned int value;  // 残機、ボム数、スコアなど。
	};

	/// <summary>テキスト表示のためのインターフェース。操作説明などを記述する。</summary>
	class IText
	{
	public:
		IText() = default;
		virtual ~IText() = default;

		virtual void SetText(std::string text)
		{
			this->text = text;
		}

		virtual void SetCommandDescription(Input::Commands command, std::string description)
		{
			commandDescriptions[command] = description;
		}
	protected:
		std::string text;
		std::map<Input::Commands, std::string> commandDescriptions;
	};

	class UserInterfaceManager : public ObjectManager
	{
	public:
		enum class UserInterfaceID {
			FrameRate,
			Title,
			Button,
			ShotKeyConfig,
			BombKeyConfig,
			SlowKeyConfig,
			SkipKeyConfig,
			LeftwardKeyConfig,
			RightwardKeyConfig,
			ForwardKeyConfig,
			BackwardKeyConfig,
			PauseKeyConfig
		};

		std::weak_ptr<UserInterface> GenerateObject(const UserInterfaceID id, const Vector2& position);
	};
}

#endif // !USER_INTERFACE_H
