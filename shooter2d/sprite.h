#ifndef SPRITE_H
#define SPRITE_H

#include <map>
#include <memory>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

namespace Shooter {
	class Sprite
	{
	public:
		Sprite(const std::shared_ptr<SDL_Texture> texture) : Sprite(texture, std::make_unique<SDL_Rect>()) {}

		Sprite(const std::shared_ptr<SDL_Texture> texture, std::unique_ptr<SDL_Rect>&& clip)
			: texture(texture)
			, clip(std::move(clip))
		{}

		void Draw(const int x, const int y) const;
		void Draw(const int x, const int y, const float angle) const;

		std::shared_ptr<SDL_Texture> GetTexture() const
		{
			return texture;
		}

		SDL_Rect& GetClip() const
		{
			return *clip;
		}

		void SetClip(const SDL_Rect& clip)
		{
			this->clip = std::make_unique<SDL_Rect>(clip);
		}
	private:
		std::shared_ptr<SDL_Texture> texture;
		std::unique_ptr<SDL_Rect> clip;
	};

	class AssetLoader
	{
	public:
		AssetLoader();
		~AssetLoader();
		std::shared_ptr<SDL_Texture> GetTexture(std::string fileName);
	private:
		std::map<std::string, std::shared_ptr<SDL_Texture>> assets;
		std::shared_ptr<SDL_Texture> addImage(std::string fileName);
	};
}

#endif // !SPRITE_H
