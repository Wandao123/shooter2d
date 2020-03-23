#ifndef MEDIA_H
#define MEDIA_H

#include "assets.h"

namespace Shooter {
	/// <summary>SDLの初期化およびウィンドウの操作を担当するクラス。</summary>
	class Media : public Singleton<Media>
	{
		Media();
		~Media();
		friend class Singleton<Media>;
	public:
		friend void Sprite::Draw() const;
		friend void Sprite::Draw(const Vector2<double>&) const;
		friend void Sprite::Draw(const Vector2<double>&, const double, const double) const;
		friend std::weak_ptr<SDL_Texture> AssetLoader::GetTexture(const std::string);
		friend void Label::Write(const Vector2<double>&) const;
		friend void Label::MakeTexture();
		friend std::weak_ptr<SDL_Texture> AssetLoader::TakeScreenshot();
		friend class Shape;
		friend void CircleShape::Draw(const Vector2<double>&) const;
		friend void RectangleShape::Draw(const Vector2<double>&) const;
		friend void BoxShape::Draw(const Vector2<double>&) const;

		int GetWidth() const
		{
			return width;
		}

		int GetHeight() const
		{
			return height;
		}

		void Clear() const;
		void ChangeScreenSize(const int width, const int height);
		void ChangeToFullScreen() const;
		bool IsFullScreen() const;
		void Present() const;
	private:
		SDL_Window *Window;
		SDL_Renderer *Renderer;
		int width = 640;
		int height = 480;
	};
}

#endif // !MEDIA_H
