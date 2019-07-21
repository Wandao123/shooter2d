#ifndef ASSETS_H
#define ASSETS_H

#include "vector2.h"
#include <map>
#include <memory>
#include <string>
#include <sstream>

/*
SDL.hを読み込む際にSDL_MAIN_HANDLEDを定義して、なおかつ初期化の際にSDL_SetMainReady()を呼ばないと
    LNK2019 未解決の外部シンボル main が関数 "int __cdecl invoke_main(void)" (?invoke_main@@YAHXZ) で参照されました。
というエラーが発生する。これはSDLの内部でmainを再定義していることに起因するらしい。このような方法の他にも、
SDL2main.libを追加の依存ファイルに指定するという方法でも対処できる。
*/
//#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

namespace Shooter {
	/// <summary>スプライトの表示を担うクラス。SDLのラッパーとして機能する。</summary>
	/// <remarks>指定した画像ファイルのテクスチャを所有する。どこを描画するかはSetClipから指定する。</remarks>
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

		void SetColor(const unsigned char red, const unsigned char green, const unsigned char blue)
		{
			SDL_SetTextureColorMod(texture.get(), red, green, blue);
		}

		Uint8 GetAlpha() const
		{
			return alpha;
		}

		void SetAlpha(const unsigned char alpha)
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

	/// <summary>テキスト表示を担うクラス。SDLのラッパーとして機能する。</summary>
	/// <remarks>現在の設計では描画する度にテクスチャを生成する。</remarks>
	class Label
	{
	public:
		Label(const std::string filename, const int size);
		std::stringstream Text;
		void Write(const Vector2& position) const;

		void SetTextColor(const unsigned char red, const unsigned char green, const unsigned char blue)
		{
			textColor = { red, green, blue, 0xFF };
		}

		void SetAlpha(const unsigned char alpha)
		{
			this->alpha = alpha;
		}
	private:
		std::weak_ptr<TTF_Font> font;
		Uint8 alpha = 0xFF;
		SDL_Color textColor = { 0xFF, 0xFF, 0xFF, 0xFF };
	};

	/// <summary>効果音 (SE) を扱うクラス。SDLのラッパーとして機能する。</summary>
	/// <remarks>短い間隔で鳴る音源のみを扱う。他の音に重ねて再生することができる。</remarks>
	class Sound
	{
	public:
		static const int MaxVolume = MIX_MAX_VOLUME;
		Sound(const std::string filename);
		void Played() const;
		
		void SetVolume(const int volume)
		{
			this->volume = volume;
			Mix_VolumeChunk(audio.lock().get(), volume);
		}
	private:
		int volume;
		std::weak_ptr<Mix_Chunk> audio;
	};

	/// <summary>音楽を扱うクラス。SDLのラッパーとして機能する。</summary>
	/// <remarks>BGMの音源のみを扱う。他の音に重ねて再生することはできない。</remarks>
	class Music
	{
	public:
		static const int MaxVolume = MIX_MAX_VOLUME;
		Music(const std::string filename);
		void Played() const;

		void SetVolume(const int volume)
		{
			this->volume = volume;
			Mix_VolumeMusic(volume);
		}
	private:
		int volume;
		std::weak_ptr<Mix_Music> audio;
	};

	/// <summary>画像・フォント・音楽の資源 (asset) を管理する。</summary>
	/// <remarks>ファイルを開いたり、そのポインタの管理はこのクラスに一任する。外から資源を使う際にはSpriteクラスなどを用いる。</remarks>
	// HACK: 外部からファイルを指定するのではなく、必要なファイルをすべてこのクラスのコンストラクタで開くべき？　その方がプレイ中に余計な処理が入らない。
	class AssetLoader
	{
	private:
		AssetLoader();
		~AssetLoader();
		static AssetLoader* instance;  // unique_ptr側からコンストラクタ・デストラクタにアクセスできないので、生ポインタを使う。
	public:
		AssetLoader(const AssetLoader&) = delete;
		AssetLoader& operator=(const AssetLoader&) = delete;
		AssetLoader(AssetLoader&&) = delete;
		AssetLoader& operator=(const AssetLoader&&) = delete;
		static AssetLoader& Create();
		static void Destroy();

		std::weak_ptr<SDL_Surface> GetImage(const std::string filename);
		std::weak_ptr<TTF_Font> GetFont(const std::string filename, const int size);
		std::weak_ptr<Mix_Chunk> GetChunk(const std::string filename);
		std::weak_ptr<Mix_Music> GetMusic(const std::string filename);
	private:
		std::map<std::string, std::shared_ptr<SDL_Surface>> images;  // Textureを共有すると、透過などの時に他の所有クラスにも影響が出る。
		std::map<std::tuple<std::string, int>, std::shared_ptr<TTF_Font>> fonts;
		std::map<std::string, std::shared_ptr<Mix_Chunk>> chunks;
		std::map<std::string, std::shared_ptr<Mix_Music>> musics;
	};
}

#endif // !ASSETS_H
