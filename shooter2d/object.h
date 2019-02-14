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

	// UI���I�u�W�F�N�g�̈��Ƃ������������邪�A�����ł͓��ɃQ�[�����̕��̂Ƃ����Ӗ��B
	// UNDONE: ����Task��Object���邢��GameObject�ɂ��āAObject��Mover�ɕύX�B
	class Object : public Task
	{
	public:
		Object(const std::string &path)
		{
			// TODO: ��O�̔����B
			// TODO: �摜�ǂݍ��ݏ����̕����BFlyweight�p�^�[�����g���B
			// TODO: �g��E�k�������B
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

		// HACK: �Q�Ƃ�Ԃ����ق��������H
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
		float highSpeed;  // �P�ʁF�h�b�g���t���[��
		float lowSpeed;
		Vector2 position = { (ScreenWidth - ImgWidth) / 2.0f, ScreenHeight - ImgHeight * 1.5f };  // �����Ō����ʒu�Ƃ͎��@�摜�̍���[�̂��ƁB
		Vector2 velocity = { 0.0f, 0.0f };
		void Move();
	};
}

#endif // !OBJECT_H
