#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "task.h"
#include "timer.h"
#include "vector2.h"

namespace Shooter {
	extern SDL_Window *Window;
	extern SDL_Renderer *Renderer;
	extern std::unique_ptr<Timer> Time;

	// UIもオブジェクトの一種という見方もあるが、ここでは特にゲーム中の物体という意味。
	// UNDONE: 今のTaskをObjectあるいはGameObjectにして、ObjectをMoverに変更。
	class Object : public Task
	{
	public:
		Object(const std::string &path)
		{
			// TODO: 例外の発生。
			// TODO: 画像読み込み処理の分離。Flyweightパターンを使う。
			// TODO: 拡大・縮小処理。
			texture = IMG_LoadTexture(Renderer, path.c_str());
			if (texture == nullptr) {
				std::cerr << "Unable to load image " << path << "!  SDL_image Error: " << IMG_GetError() << std::endl;
			}
		}

		~Object()
		{
			SDL_DestroyTexture(texture);
		}

	protected:
		SDL_Texture *texture;
	};

	class Player : public Object
	{
	public:
		Player(const std::string &path, const float highSpeed, const float lowSpeed);
		~Player() = default;
		void Draw();
		void Update();

		// HACK: 参照を返したほうがいい？
		Vector2 GetPosition()
		{
			return position;
		}

		Vector2 GetVelocity()
		{
			return velocity;
		}

	private:
		const int ImgWidth = 32;
		const int ImgHeight = 48;
		SDL_Rect clips[3][5];
		float highSpeed;  // 単位：ドット毎フレーム
		float lowSpeed;
		Vector2 position = { (ScreenWidth - ImgWidth) / 2.0f, ScreenHeight - ImgHeight * 1.5f };  // ここで言う位置とは自機画像の左上端のこと。
		Vector2 velocity = { 0.0f, 0.0f };
		void Move();
	};
}

#endif // !OBJECT_H
