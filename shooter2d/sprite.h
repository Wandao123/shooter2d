#ifndef SPRITE_H
#define SPRITE_H

#include <map>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

namespace Shooter {
	extern SDL_Renderer *Renderer;

	class Sprite
	{
	public:
		Sprite(const std::shared_ptr<SDL_Texture> texture) : Sprite(texture, std::make_shared<SDL_Rect>()) {}

		Sprite(const std::shared_ptr<SDL_Texture> texture, const std::shared_ptr<SDL_Rect> Clip)
			: texture(texture)
			, clip(Clip)
		{}

		void Draw(const int x, const int y) const;

		std::shared_ptr<SDL_Texture> GetTexture() const
		{
			return texture;
		}

		std::shared_ptr<SDL_Rect> GetClip() const
		{
			return clip;
		}

		void SetClip(const std::shared_ptr<SDL_Rect> clip)
		{
			this->clip = clip;
		}
	private:
		std::shared_ptr<SDL_Texture> texture;
		std::shared_ptr<SDL_Rect> clip;
	};

	class AssetLoader
	{
	public:
		std::shared_ptr<SDL_Texture> GetTexture(std::string fileName);
	private:
		std::map<std::string, std::shared_ptr<SDL_Texture>> assets;
		std::shared_ptr<SDL_Texture> addImage(std::string fileName);
	};
}

#endif // !SPRITE_H
