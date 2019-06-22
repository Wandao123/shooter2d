#ifndef SPRITE_H
#define SPRITE_H

#include <map>
#include <memory>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include "vector2.h"

namespace Shooter {
	class Sprite
	{
	public:
		Sprite(const std::shared_ptr<SDL_Texture> texture) : texture(texture) {}
		void Draw(const Vector2& position) const;
		void Draw(const Vector2& position, const float angle, const float scale) const;

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

		void SetColor(const Uint8 red, const Uint8 green, const Uint8 blue)
		{
			SDL_SetTextureColorMod(texture.get(), red, green, blue);
		}

		Uint8 GetAlpha() const
		{
			return alpha;
		}

		void SetAlpha(const Uint8 alpha)
		{
			SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(texture.get(), alpha);
			this->alpha = alpha;
		}
	private:
		std::shared_ptr<SDL_Texture> texture;
		std::unique_ptr<SDL_Rect> clip;
		Uint8 alpha = 255;
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
