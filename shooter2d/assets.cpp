#include <iostream>
#include "assets.h"

namespace Shooter {
	extern SDL_Renderer* Renderer;
}

using namespace Shooter;

/******************************** Sprite *********************************/

Sprite::Sprite(const std::weak_ptr<SDL_Texture> texture)
	: texture(texture)
{
	int width, height;
	SDL_QueryTexture(this->texture.lock().get(), nullptr, nullptr, &width, &height);
	clip = std::make_unique<SDL_Rect>(SDL_Rect{ 0, 0, width, height });
}

/// <param name="x">描画する位置のx座標</param>
/// <param name="y">描画する位置のy座標</param>
/// <remarks>ここでいうpositionとは描画するテキストの中心位置。</remarks>
void Sprite::Draw(const Vector2<float>& position) const
{
	SDL_Rect renderClip = { static_cast<int>(position.x - clip->w * 0.5f), static_cast<int>(position.y - clip->h * 0.5f), clip->w, clip->h };
	SDL_RenderCopy(Renderer, texture.lock().get(), clip.get(), &renderClip);  // エラーコードを受け取った方が良い？
}

/// <param name="x">描画する位置のx座標</param>
/// <param name="y">描画する位置のy座標</param>
/// <param name="angle">回転角（弧度法）</param>
/// <param name="scale">拡大・縮小率 (0.0 .. 1.0)</param>
/// <remarks>ここでいうpositionとは描画するテキストの中心位置。回転の中心は (clip->w / 2, clip->h / 2)。</remarks>
void Sprite::Draw(const Vector2<float>& position, const float angle, const float scale) const
{
	int scaledWidth = static_cast<int>(clip->w * scale);
	int scaledHeight = static_cast<int>(clip->h * scale);
	SDL_Rect renderClip = { static_cast<int>(position.x - scaledWidth * 0.5f), static_cast<int>(position.y - scaledHeight * 0.5f), scaledWidth, scaledHeight };
	SDL_RenderCopyEx(Renderer, texture.lock().get(), clip.get(), &renderClip, angle * 180.0 / M_PI, nullptr, SDL_FLIP_NONE);  // エラーコードを受け取った方が良い？
}

/******************************** Label *********************************/

Label::Label(const std::weak_ptr<TTF_Font> font)
	: Text(" ")  // Textが空のままだと、いきなりWriteが呼ばれてエラー。
	, font(font)
	//, texture(nullptr, nullptr)
{}

/// <param name="x">描画する位置のx座標</param>
/// <param name="y">描画する位置のy座標</param>
/// <remarks>ここでいうpositionとは描画するテキストの中心位置。</remarks>
void Label::Write(const Vector2<float>& position) const
{
	SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font.lock().get(), Text.c_str(), textColor);
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

/* 色を変える場合も同じような処理を書かなければならないため、結局無駄？
void Label::SetText(const std::string text)
{
	this->text = text;

	SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font.lock().get(), text.c_str(), textColor);
	if (textSurface == nullptr) {
		std::cerr << TTF_GetError() << std::endl;
		return;
	}
	SDL_Texture* rawTexture = SDL_CreateTextureFromSurface(Renderer, textSurface);
	if (rawTexture == nullptr) {
		std::cerr << SDL_GetError() << std::endl;
	}
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture(rawTexture, SDL_DestroyTexture);
	this->texture = std::move(texture);
}*/

/******************************** Sound *********************************/

Sound::Sound(const std::weak_ptr<Mix_Chunk> audio)
	: audio(audio)
	, volume(0)
{}

void Sound::Played() const
{
	Mix_PlayChannel(-1, audio.lock().get(), 0);  // エラーコードを受け取った方が良い？
}

void Sound::SetVolume(const int volume)
{
	this->volume = volume;
	Mix_VolumeChunk(audio.lock().get(), std::round(static_cast<float>(AssetLoader::Create().GetSoundVolume()) / 100 * volume));
}

/******************************** Music *********************************/

Music::Music(const std::weak_ptr<Mix_Music> audio)
	: audio(audio)
	, volume(0)
{}

void Music::Played() const
{
	Mix_PlayMusic(audio.lock().get(), 0);  // エラーコードを受け取った方が良い？
}

void Music::SetVolume(const int volume)
{
	this->volume = volume;
	Mix_VolumeMusic(std::round(static_cast<float>(AssetLoader::Create().GetMusicVolume()) / 100 * volume));
}

/******************************** AssetLoader *********************************/

AssetLoader::AssetLoader()
	: soundVolume(80)
	, musicVolume(100)
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
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cerr << "SDL_mixer could not initialize!SDL_mixer Error :" << Mix_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	Mix_AllocateChannels(256);
}

AssetLoader::~AssetLoader()
{
	surfaces.clear();
	textures.clear();
	fonts.clear();
	chunks.clear();
	musics.clear();
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
}

std::weak_ptr<SDL_Surface> AssetLoader::GetSurface(const std::string filename)
{
	auto addImage = [](const std::string filename) -> std::shared_ptr<SDL_Surface> {
		SDL_Surface* rawSurface = IMG_Load(filename.c_str());  // TODO: 例外の発生。
		if (rawSurface == nullptr) {
			std::cerr << "Unable to load image " << filename << "!  SDL_image Error: " << IMG_GetError() << std::endl;
		}
		std::shared_ptr<SDL_Surface> surface(rawSurface, SDL_FreeSurface);
		return surface;
	};

	auto iter = surfaces.find(filename);
	if (iter != surfaces.end()) {
		return iter->second;
	} else {
		auto newImage = addImage(filename);
		surfaces[filename] = newImage;
		return newImage;
	}
}

// このメンバ関数のみはプーリング処理をしない（使う側に任せる）。
std::weak_ptr<SDL_Texture> AssetLoader::GetTexture(const std::string filename)
{
	auto addImage = [this](const std::string filename) -> std::shared_ptr<SDL_Texture> {
		SDL_Texture* rawTexture = SDL_CreateTextureFromSurface(Renderer, GetSurface(filename).lock().get());
		if (rawTexture == nullptr) {
			std::cerr << "Unable to create texture from " << filename << "! SDL Error: " << SDL_GetError();
		}
		std::shared_ptr<SDL_Texture> texture(rawTexture, SDL_DestroyTexture);
		return texture;
	};

	auto newImage = addImage(filename);
	textures.insert(std::make_pair(filename, newImage));
	return newImage;
}

std::weak_ptr<TTF_Font> AssetLoader::GetFont(const std::string filename, const int size)
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
	} else {
		auto newFont = addFont(filename, size);
		fonts[key] = newFont;
		return newFont;
	}
}

std::weak_ptr<Mix_Chunk> AssetLoader::GetChunk(const std::string filename)
{
	auto addChunk = [](const std::string filename) -> std::shared_ptr<Mix_Chunk> {
		Mix_Chunk* rawChunk = Mix_LoadWAV(filename.c_str());  // TODO: 例外の発生。
		if (rawChunk == nullptr) {
			std::cerr << "Failed to load " << filename << " sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
		}
		std::shared_ptr<Mix_Chunk> chunk(rawChunk, Mix_FreeChunk);
		return chunk;
	};

	auto iter = chunks.find(filename);
	if (iter != chunks.end()) {
		return iter->second;
	} else {
		auto newChunk = addChunk(filename);
		chunks[filename] = newChunk;
		return newChunk;
	}
}

std::weak_ptr<Mix_Music> AssetLoader::GetMusic(const std::string filename)
{
	auto addMusic = [](const std::string filename) -> std::shared_ptr<Mix_Music> {
		Mix_Music* rawMusic = Mix_LoadMUS(filename.c_str());  // TODO: 例外の発生。
		if (rawMusic == nullptr) {
			std::cerr << "Failed to load " << filename << " music! SDL_mixer Error: " << Mix_GetError() << std::endl;
		}
		std::shared_ptr<Mix_Music> music(rawMusic, Mix_FreeMusic);
		return music;
	};

	auto iter = musics.find(filename);
	if (iter != musics.end()) {
		return iter->second;
	} else {
		auto newMusic = addMusic(filename);
		musics[filename] = newMusic;
		return newMusic;
	}
}
