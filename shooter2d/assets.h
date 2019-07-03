#ifndef ASSETS_H
#define ASSETS_H

#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
//#include <SDL_mixer.h>
#include "vector2.h"

namespace Shooter {
	class Sprite
	{
	public:
		Sprite(const std::string filename);
		void Draw(const Vector2& position) const;
		void Draw(const Vector2& position, const float angle, const float scale) const;

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

		void SetBlendModeAdd()
		{
			SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_ADD);
		}

		void SetBlendModeMod()
		{
			SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_MOD);
		}
	private:
		std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture;
		std::unique_ptr<SDL_Rect> clip;
		Uint8 alpha = 0xFF;
	};

	class Label
	{
	public:
		Label(const std::string filename, const int size);
		std::stringstream Text;
		void Write(const Vector2& position) const;

		void SetTextColor(const Uint8 red, const Uint8 green, const Uint8 blue)
		{
			textColor = { red, green, blue };
		}

		void SetAlpha(const Uint8 alpha)
		{
			this->alpha = alpha;
		}
	private:
		std::weak_ptr<TTF_Font> font;
		Uint8 alpha = 0xFF;
		SDL_Color textColor = { 0xFF, 0xFF, 0xFF };
	};

	/// <summary>画像・フォント・音楽の資源 (asset) を管理する。</summary>
	/// <remarks>ファイルを開いたり、そのポインタの管理はこのクラスに一任する。外から資源を使う際にはSpriteクラスなどを用いる。</remarks>
	// HACK: 外部からファイルを指定するのではなく、必要なファイルをすべてこのクラスのコンストラクタで開くべき？　その方がプレイ中に余計な処理が入らない。
	class CAssetLoader
	{
	public:
		CAssetLoader();
		~CAssetLoader();
		std::weak_ptr<SDL_Surface> GetImage(const std::string filename);
		std::weak_ptr<TTF_Font> GetFont(const std::string filename, const int size);
	private:
		std::map<std::string, std::shared_ptr<SDL_Surface>> images;  // Textureを共有すると、透過などの時に他の所有クラスにも影響が出る。
		std::map<std::tuple<std::string, int>, std::shared_ptr<TTF_Font>> fonts;
	};
}

#endif // !ASSETS_H
