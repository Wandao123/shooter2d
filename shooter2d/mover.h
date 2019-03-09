#ifndef MOVER_H
#define MOVER_H

#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "game_object.h"
#include "timer.h"
#include "vector2.h"

namespace Shooter {
	extern SDL_Window *Window;
	extern SDL_Renderer *Renderer;
	extern std::unique_ptr<Timer> Time;

	class Mover : public GameObject
	{
	public:
		Mover(const std::string &path)
		{
			// TODO: 例外の発生。
			// TODO: 画像読み込み処理の分離。Flyweightパターンを使う。
			// TODO: 拡大・縮小処理。
			texture = IMG_LoadTexture(Renderer, path.c_str());
			if (texture == nullptr) {
				std::cerr << "Unable to load image " << path << "!  SDL_image Error: " << IMG_GetError() << std::endl;
			}
		}

		// TODO: 仮想デストラクタが必須？
		~Mover()
		{
			SDL_DestroyTexture(texture);
		}

	protected:
		SDL_Texture *texture;
	};

	class Player : public Mover
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
		// TODO: 回転角もプロパティとして持つ。
		void Move();
	};
}

#endif // !MOVER_H
