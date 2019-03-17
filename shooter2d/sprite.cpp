#include <iostream>
#include "sprite.h"

namespace Shooter {
	extern SDL_Renderer *Renderer;

	void Sprite::Draw(const int x, const int y) const
	{
		SDL_Rect renderClip = { x, y, clip->w, clip->h };
		SDL_RenderCopy(Renderer, texture.get(), clip.get(), &renderClip);
	}

	AssetLoader::AssetLoader()
	{
		int imgFlags = IMG_INIT_PNG;
		if (!(IMG_Init(imgFlags) & imgFlags)) {
			std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	AssetLoader::~AssetLoader()
	{
		IMG_Quit();
	}

	std::shared_ptr<SDL_Texture> AssetLoader::GetTexture(std::string fileName)
	{
		auto iter = assets.find(fileName);
		if (iter != assets.end()) {
			return iter->second;
		} else {
			auto newTexture = addImage(fileName);
			assets[fileName] = newTexture;
			return newTexture;
		}
	}

	std::shared_ptr<SDL_Texture> AssetLoader::addImage(std::string fileName)
	{
		// TODO: 例外の発生。
		// TODO: 拡大・縮小処理。
		SDL_Texture* texture = IMG_LoadTexture(Renderer, fileName.c_str());
		if (texture == nullptr) {
			std::cerr << "Unable to load image " << fileName << "!  SDL_image Error: " << IMG_GetError() << std::endl;
		}
		std::shared_ptr<SDL_Texture> pTexture(texture, SDL_DestroyTexture);
		return pTexture;
	}
}