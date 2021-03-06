#ifndef ASSETS_H
#define ASSETS_H

#include "vector2.h"
#include "singleton.h"
#include <list>
#include <map>
#include <memory>
#include <string>

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
		Sprite(const std::weak_ptr<SDL_Texture> texture);
		void Draw() const;
		void Draw(const Vector2<double>& position) const;
		void Draw(const Vector2<double>& position, const double angle, const double scale) const;

		Rect<int> GetClip() const
		{
			return { clip->x, clip->y, clip->w, clip->h };
		}

		void SetClip(const Rect<int>& clip)
		{
			this->clip = std::make_unique<SDL_Rect>(SDL_Rect{ clip.x, clip.y, clip.width, clip.height });
		}

		// TODO: SetColorとSetAlphaとを統合。
		void SetColor(const unsigned char red, const unsigned char green, const unsigned char blue)
		{
			SDL_SetTextureColorMod(texture.lock().get(), red, green, blue);
		}

		Uint8 GetAlpha() const
		{
			return alpha;
		}

		void SetAlpha(const unsigned char alpha)
		{
			SDL_SetTextureBlendMode(texture.lock().get(), SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(texture.lock().get(), alpha);
			this->alpha = alpha;
		}

		void SetBlendModeAdd()
		{
			SDL_SetTextureBlendMode(texture.lock().get(), SDL_BLENDMODE_ADD);
		}

		void SetBlendModeMod()
		{
			SDL_SetTextureBlendMode(texture.lock().get(), SDL_BLENDMODE_MOD);
		}
	private:
		std::weak_ptr<SDL_Texture> texture;
		std::unique_ptr<SDL_Rect> clip;
		Uint8 alpha = 0xFF;
	};

	/// <summary>テキスト表示を担うクラス。SDLのラッパーとして機能する。</summary>
	/// <remarks>現在の設計では描画する度にテクスチャを生成する。</remarks>
	class Label
	{
	public:
		std::string Text;
		Label(const std::weak_ptr<TTF_Font> font);
		void Write(const Vector2<double>& position) const;
		//void SetText(const std::string text);

		// TODO: SetTextColorとSetAlphaとを統合。
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
		//std::string text;
		//std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture;
		Uint8 alpha = 0xFF;
		SDL_Color textColor = { 0xFF, 0xFF, 0xFF, 0xFF };
	};

	/// <summary>効果音 (SE) を扱うクラス。SDLのラッパーとして機能する。</summary>
	/// <remarks>短い間隔で鳴る音源のみを扱う。他の音に重ねて再生することができる。</remarks>
	class Sound
	{
	public:
		static const int MaxVolume = MIX_MAX_VOLUME;
		Sound(const std::weak_ptr<Mix_Chunk> audio);
		void Played() const;
		void SetVolume(const int volume);
	private:
		std::weak_ptr<Mix_Chunk> audio;
		int volume;
	};

	/// <summary>音楽を扱うクラス。SDLのラッパーとして機能する。</summary>
	/// <remarks>BGMの音源のみを扱う。他の音に重ねて再生することはできない。</remarks>
	class Music
	{
	public:
		static const int MaxVolume = MIX_MAX_VOLUME;
		Music(const std::weak_ptr<Mix_Music> audio);
		void Played() const;
		void SetVolume(const int volume);
	private:
		std::weak_ptr<Mix_Music> audio;
		int volume;
	};

	/// <summary>画像・フォント・音楽の資源 (asset) を管理する。</summary>
	/// <remarks>ファイルを開いたり、そのポインタの管理はこのクラスに一任する。外から資源を使う際にはSpriteクラスなどを用いる。</remarks>
	// HACK: 外部からファイルを指定するのではなく、必要なファイルをすべてこのクラスのコンストラクタで開くべき？　その方がプレイ中に余計な処理が入らない。
	class AssetLoader : public Singleton<AssetLoader>
	{
		AssetLoader();
		~AssetLoader();
		friend class Singleton<AssetLoader>;
	public:
		std::weak_ptr<SDL_Surface> GetSurface(const std::string filename);
		std::weak_ptr<SDL_Texture> GetTexture(const std::string filename);
		std::weak_ptr<TTF_Font> GetFont(const std::string filename, const int size);
		std::weak_ptr<Mix_Chunk> GetChunk(const std::string filename);
		std::weak_ptr<Mix_Music> GetMusic(const std::string filename);
		std::weak_ptr<SDL_Texture> TakeScreenshot();

		unsigned short int GetSoundVolume()
		{
			return soundVolume;
		}

		/// <summary>Soundの音量を調節する。</summary>
		/// <param name="parcentage">Sound::SetVolumeの音量の割合（百分率）</param>
		void SetSoundVolume(const unsigned short int percentage)
		{
			soundVolume = percentage;
		}
		
		unsigned short int GetMusicVolume()
		{
			return musicVolume;
		}

		/// <summary>Musicの音量を調節する。</summary>
		/// <param name="parcentage">Music::SetVolumeの音量の割合（百分率）</param>
		void SetMusicVolume(const unsigned short int percentage)
		{
			musicVolume = percentage;
		}

		void ClearScreenshots()
		{
			screenshots.clear();
		}
	private:
		std::map<std::string, std::shared_ptr<SDL_Surface>> surfaces;
		std::multimap<std::string, std::shared_ptr<SDL_Texture>> textures;  // Textureを共有すると、透過などの際に他のクラスにも影響が出ることに注意。
		std::map<std::tuple<std::string, int>, std::shared_ptr<TTF_Font>> fonts;
		std::map<std::string, std::shared_ptr<Mix_Chunk>> chunks;
		std::map<std::string, std::shared_ptr<Mix_Music>> musics;
		std::list<std::shared_ptr<SDL_Texture>> screenshots;
		unsigned short int soundVolume;
		unsigned short int musicVolume;
	};

	/// <summary>図形の描画を担うクラス。SDLのラッパーとして機能する。</summary>
	class Shape
	{
	public:
		Shape() = default;
		virtual ~Shape() = default;
		virtual void Draw(const Vector2<int>&) const = 0;
		void SetColor(const unsigned char red, const unsigned char green, const unsigned char blue, const unsigned char alpha);
		void SetBlendModeAdd();
		void SetBlendModeMod();
	protected:
		SDL_Color color = { 0xFF, 0xFF, 0xFF, 0xFF };
	};

	class RectangleShape : public Shape
	{
	public:
		RectangleShape(const Vector2<int>& size = Vector2<int>(0, 0));
		void Draw(const Vector2<int>& position) const override;

		const Vector2<int>& GetSize() const
		{
			return size;
		}

		void SetSize(const Vector2<int>& size)
		{
			this->size = size;
		}
	private:
		Vector2<int> size;
	};
}

#endif // !ASSETS_H
