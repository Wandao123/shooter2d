#include <iostream>
#include "assets.h"

namespace Shooter {
	extern SDL_Renderer* Renderer;
	extern std::unique_ptr<CAssetLoader> AssetLoader;
}

using namespace Shooter;

/******************************** Sprite *********************************/

Sprite::Sprite(const std::string filename)
	: texture(AssetLoader->GetImage(filename))
{
	// clipメンバ変数の初期値をtexture全体にする。
	int width, height;
	SDL_QueryTexture(texture.lock().get(), nullptr, nullptr, &width, &height);
	SDL_Rect temp = { 0, 0, width, height };
	SetClip(temp);
}

/// <param name="x">描画する位置のx座標</param>
/// <param name="y">描画する位置のy座標</param>
/// <remarks>ここでいうpositionとは描画するテキストの中心位置。</remarks>
void Sprite::Draw(const Vector2& position) const
{
	SDL_Rect renderClip = { static_cast<int>(position.x - clip->w * 0.5f), static_cast<int>(position.y - clip->h * 0.5f), clip->w, clip->h };
	SDL_RenderCopy(Renderer, texture.lock().get(), clip.get(), &renderClip);  // エラーコードを受け取った方が良い？
}

/// <param name="x">描画する位置のx座標</param>
/// <param name="y">描画する位置のy座標</param>
/// <param name="angle">回転角（弧度法）</param>
/// <param name="scale">拡大・縮小率 (0.0 .. 1.0)</param>
/// <remarks>ここでいうpositionとは描画するテキストの中心位置。回転の中心は (clip->w / 2, clip->h / 2)。</remarks>
void Sprite::Draw(const Vector2& position, const float angle, const float scale) const
{
	int scaledWidth = static_cast<int>(clip->w * scale);
	int scaledHeight = static_cast<int>(clip->h * scale);
	SDL_Rect renderClip = { static_cast<int>(position.x - scaledWidth * 0.5f), static_cast<int>(position.y - scaledHeight * 0.5f), scaledWidth, scaledHeight };
	SDL_RenderCopyEx(Renderer, texture.lock().get(), clip.get(), &renderClip, angle * 180.0 / M_PI, nullptr, SDL_FLIP_NONE);  // エラーコードを受け取った方が良い？
}

/******************************** Label *********************************/

Label::Label(const std::string filename, const int size)
	: font(AssetLoader->GetFont(filename, size))
{
	Text.str(" ");  // Textが空のままだと、いきなりWriteが呼ばれてエラー。
}

/// <param name="x">描画する位置のx座標</param>
/// <param name="y">描画する位置のy座標</param>
/// <remarks>ここでいうpositionとは描画するテキストの中心位置。</remarks>
void Label::Write(const Vector2& position) const
{
	SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font.lock().get(), Text.str().c_str(), textColor);
	if (textSurface == nullptr) {
		std::cerr << TTF_GetError() << std::endl;
		return;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(Renderer, textSurface);
	if (texture == nullptr) {
		std::cerr << SDL_GetError() << std::endl;
		return;
	}
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(texture, alpha);
	SDL_Rect renderText = {
		static_cast<int>(position.x - textSurface->w * 0.5f),
		static_cast<int>(position.y - textSurface->h * 0.5f),
		textSurface->w,
		textSurface->h
	};
	SDL_RenderCopy(Renderer, texture, nullptr, &renderText);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(textSurface);
}

/******************************** AssetLoader *********************************/

CAssetLoader::CAssetLoader()
{
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	if (TTF_Init() == -1) {
		std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
}

CAssetLoader::~CAssetLoader()
{
	images.clear();
	fonts.clear();
	TTF_Quit();
	IMG_Quit();
}

std::weak_ptr<SDL_Texture> CAssetLoader::GetImage(const std::string filename)
{
	auto addImage = [](const std::string filename) -> std::shared_ptr<SDL_Texture> {
		SDL_Texture* rawTexture = IMG_LoadTexture(Renderer, filename.c_str());  // TODO: 例外の発生。
		if (rawTexture == nullptr) {
			std::cerr << "Unable to load image " << filename << "!  SDL_image Error: " << IMG_GetError() << std::endl;
		}
		std::shared_ptr<SDL_Texture> texture(rawTexture, SDL_DestroyTexture);
		return texture;
	};

	auto iter = images.find(filename);
	if (iter != images.end()) {
		return iter->second;
	} else {
		auto newImage = addImage(filename);
		images[filename] = newImage;
		return newImage;
	}
}

std::weak_ptr<TTF_Font> CAssetLoader::GetFont(const std::string filename, const int size)
{
	auto addFont = [](const std::string filename, const int size) -> std::shared_ptr<TTF_Font> {
		TTF_Font* rawFont = TTF_OpenFont(filename.c_str(), size);  // TODO: 例外の発生。
		if (rawFont == nullptr) {
			std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
		}
		std::shared_ptr<TTF_Font> font(rawFont, TTF_CloseFont);
		return font;
	};

	auto key = std::make_tuple(filename, size);
	auto iter = fonts.find(key);
	if (iter != fonts.end()) {
		return iter->second;
	}
	else {
		auto newFont = addFont(filename, size);
		fonts[key] = newFont;
		return newFont;
	}
}