#include <iostream>
#include "sprite.h"
#include "vector2.h"

namespace Shooter {
	extern SDL_Renderer *Renderer;

	/// <param name="x">描画する位置の左上端のx座標</param>
	/// <param name="y">描画する位置の左上端のy座標</param>
	void Sprite::Draw(const int x, const int y) const
	{
		SDL_Rect renderClip = { x, y, clip->w, clip->h };
		SDL_RenderCopy(Renderer, texture.get(), clip.get(), &renderClip);  // エラーコードを受け取った方が良い？
	}

	/// <param name="x">描画する位置の左上端のx座標</param>
	/// <param name="y">描画する位置の左上端のy座標</param>
	/// <param name="angle">回転角（弧度法）</param>
	// 回転の中心は (clip->w / 2, clip->h / 2)
	void Sprite::Draw(const int x, const int y, const float angle) const
	{
		SDL_Rect renderClip = { x, y, clip->w, clip->h };
		SDL_RenderCopyEx(Renderer, texture.get(), clip.get(), &renderClip, angle * 180.0 / M_PI, nullptr, SDL_FLIP_NONE);  // エラーコードを受け取った方が良い？
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