#include <array>
#include <iostream>
#include "media.h"  // assets.hは読み込み済み。

using namespace Shooter;

/******************************** Sprite *********************************/

Sprite::Sprite(const std::weak_ptr<SDL_Texture> texture)
	: texture(texture)
{
	int width, height;
	SDL_QueryTexture(this->texture.lock().get(), nullptr, nullptr, &width, &height);
	clip = std::make_unique<SDL_Rect>(SDL_Rect{ 0, 0, width, height });
}

/// <summary>textureをそのまま描画する。</summary>
void Sprite::Draw() const
{
	SDL_RenderCopy(Media::Create().Renderer, texture.lock().get(), nullptr, nullptr);  // エラーコードを受け取った方が良い？
}

/// <summary>textureのclipで切り抜いた部分を描画する。</summary>
/// <param name="x">描画する位置のx座標</param>
/// <param name="y">描画する位置のy座標</param>
/// <remarks>ここでいうpositionとは描画するテキストの中心位置。</remarks>
void Sprite::Draw(const Vector2<double>& position) const
{
	SDL_Rect renderClip = { static_cast<int>(position.x - clip->w * 0.5), static_cast<int>(position.y - clip->h * 0.5), clip->w, clip->h };
	SDL_RenderCopy(Media::Create().Renderer, texture.lock().get(), clip.get(), &renderClip);  // エラーコードを受け取った方が良い？
}

/// <summary>textureのclipで切り抜いた部分を描画する。</summary>
/// <param name="x">描画する位置のx座標</param>
/// <param name="y">描画する位置のy座標</param>
/// <param name="angle">回転角（弧度法）</param>
/// <param name="scale">拡大・縮小率 (0.0 .. 1.0)</param>
/// <remarks>ここでいうpositionとは描画するテキストの中心位置。回転の中心は (clip->w / 2, clip->h / 2)。</remarks>
void Sprite::Draw(const Vector2<double>& position, const double angle, const double scale) const
{
	int scaledWidth = static_cast<int>(clip->w * scale);
	int scaledHeight = static_cast<int>(clip->h * scale);
	SDL_Rect renderClip = { static_cast<int>(position.x - scaledWidth * 0.5), static_cast<int>(position.y - scaledHeight * 0.5), scaledWidth, scaledHeight };
	SDL_RenderCopyEx(Media::Create().Renderer, texture.lock().get(), clip.get(), &renderClip, angle * 180.0 / M_PI, nullptr, SDL_FLIP_NONE);  // エラーコードを受け取った方が良い？
}

/******************************** Label *********************************/

Label::Label(const std::weak_ptr<TTF_Font> font)
	: font(font)
	, texture(nullptr, nullptr)
	, text(" ")  // textが空のままいきなりMakeTextureを呼ぶとエラー。
	, color({ 0xFF, 0xFF, 0xFF, 0xFF })
	, width(0)
	, height(0)
{}

/// <summary>Textに入っている文字列を描画する。</summary>
/// <param name="x">描画する位置のx座標</param>
/// <param name="y">描画する位置のy座標</param>
/// <remarks>ここでいうpositionとは描画するテキストの中心位置。</remarks>
void Label::Write(const Vector2<double>& position) const
{
	if (texture) {
		SDL_Rect renderText = {
			static_cast<int>(position.x - width * 0.5),
			static_cast<int>(position.y - height * 0.5),
			width,
			height
		};
		SDL_RenderCopy(Media::Create().Renderer, texture.get(), nullptr, &renderText);
	}
}

/// <summary>設定されたtextとcolorとからtextureを生成する。</summary>
void Label::MakeTexture()
{
	SDL_Surface* textSurface = TTF_RenderUTF8_Blended(font.lock().get(), text.c_str(), color);
	if (textSurface == nullptr) {
		std::cerr << TTF_GetError() << std::endl;
		return;
	}
	SDL_SetColorKey(textSurface, SDL_TRUE, SDL_MapRGB(textSurface->format, 0x00, 0x00, 0x00));  // 必要か否かよく判らない。
	SDL_Texture* rawTexture = SDL_CreateTextureFromSurface(Media::Create().Renderer, textSurface);
	if (rawTexture == nullptr) {
		std::cerr << SDL_GetError() << std::endl;
	}
	std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)> texture(rawTexture, SDL_DestroyTexture);
	SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);
	this->texture = std::move(texture);
	width = textSurface->w;
	height = textSurface->h;
	SDL_FreeSurface(textSurface);
}

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
	Mix_VolumeChunk(audio.lock().get(), static_cast<int>(static_cast<double>(AssetLoader::Create().GetSoundVolume()) / 100 * volume));
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
	Mix_VolumeMusic(static_cast<int>(static_cast<double>(AssetLoader::Create().GetMusicVolume()) / 100 * volume));
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
		SDL_Texture* rawTexture = SDL_CreateTextureFromSurface(Media::Create().Renderer, GetSurface(filename).lock().get());
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

std::weak_ptr<SDL_Texture> AssetLoader::TakeScreenshot()
{
	auto addImage = [this]() -> std::shared_ptr<SDL_Texture> {
		int width, height;
		SDL_GetRendererOutputSize(Media::Create().Renderer, &width, &height);
		SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
		SDL_RenderReadPixels(Media::Create().Renderer, nullptr, surface->format->format, surface->pixels, surface->pitch);
		SDL_Texture* rawTexture = SDL_CreateTextureFromSurface(Media::Create().Renderer, surface);
		SDL_FreeSurface(surface);
		std::shared_ptr<SDL_Texture> texture(rawTexture, SDL_DestroyTexture);
		return texture;
	};

	auto newImage = addImage();
	screenshots.push_back(newImage);
	return newImage;
}

/******************************** Shape *********************************/

void Shape::predraw() const
{
	switch (blendMode){
	case BlendMode::None:
		SDL_SetRenderDrawBlendMode(Media::Create().Renderer, SDL_BLENDMODE_NONE);
		break;
	case BlendMode::Blend:
		SDL_SetRenderDrawBlendMode(Media::Create().Renderer, SDL_BLENDMODE_BLEND);
		break;
	case BlendMode::Add:
		SDL_SetRenderDrawBlendMode(Media::Create().Renderer, SDL_BLENDMODE_ADD);
		break;
	case BlendMode::Mod:
		SDL_SetRenderDrawBlendMode(Media::Create().Renderer, SDL_BLENDMODE_MOD);
		break;
	}
}

void Shape::postdraw() const
{
	SDL_SetRenderDrawBlendMode(Media::Create().Renderer, SDL_BLENDMODE_NONE);
}

/******************************** CircleShape *********************************/

CircleShape::CircleShape(const int radius)
	: radius(radius)
{}

void CircleShape::Draw(const Vector2<double>& position) const
{
	predraw();
	circleRGBA(Media::Create().Renderer,
		static_cast<Sint16>(position.x),
		static_cast<Sint16>(position.y),
		static_cast<Sint16>(radius),
		color.r, color.g, color.b, color.a);
	postdraw();
}

/******************************** RectangleShape *********************************/

RectangleShape::RectangleShape(const Vector2<double>& size)
	: size(size)
{}

/// <summary>矩形を描画する。</summary>
/// <param name="position">矩形の中心の座標</param>
/// <remarks>内部は塗り潰す。</remarks>
void RectangleShape::Draw(const Vector2<double>& position) const
{
	predraw();
	if (angle == 0.e0 || angle == M_PI) {
		//SDL_SetRenderDrawColor(Media::Create().Renderer, color.r, color.g, color.b, color.a);
		//SDL_Rect rect = { position.x - size.x / 2, position.y - size.y / 2, size.x, size.y };
		//SDL_RenderRect(Media::Create().Renderer, &rect);
		rectangleRGBA(Media::Create().Renderer,
			static_cast<Sint16>(position.x + size.x / 2),
			static_cast<Sint16>(position.y - size.y / 2),
			static_cast<Sint16>(position.x - size.x / 2),
			static_cast<Sint16>(position.y + size.y / 2),
			color.r, color.g, color.b, color.a);
	} else {
		/*std::array<Vector2<int>, 4> vertices = {
			((-size) / 2).Rotate(angle),
			(Vector2<int>{ -size.x, size.y } / 2).Rotate(angle),
			(size / 2).Rotate(angle),
			(Vector2<int>{ size.x, -size.y } / 2).Rotate(angle)
		};
		lineRGBA(Media::Create().Renderer, vertices[0].x, vertices[0].y, vertices[1].x, vertices[1].y, color.r, color.g, color.b, color.a);
		lineRGBA(Media::Create().Renderer, vertices[1].x, vertices[1].y, vertices[2].x, vertices[2].y, color.r, color.g, color.b, color.a);
		lineRGBA(Media::Create().Renderer, vertices[2].x, vertices[2].y, vertices[3].x, vertices[3].y, color.r, color.g, color.b, color.a);
		lineRGBA(Media::Create().Renderer, vertices[3].x, vertices[3].y, vertices[0].x, vertices[0].y, color.r, color.g, color.b, color.a);*/
		std::array<Vector2<double>, 4> vertices = {
			position + (-size / 2).Rotate(angle),
			position + (Vector2<double>{ -size.x, size.y } / 2).Rotate(angle),
			position + (size / 2).Rotate(angle),
			position + (Vector2<double>{ size.x, -size.y } / 2).Rotate(angle)
		};
		std::array<Sint16, 4> vx = {
			static_cast<Sint16>(vertices[0].x),
			static_cast<Sint16>(vertices[1].x),
			static_cast<Sint16>(vertices[2].x),
			static_cast<Sint16>(vertices[3].x)
		};
		std::array<Sint16, 4> vy = {
			static_cast<Sint16>(vertices[0].y),
			static_cast<Sint16>(vertices[1].y),
			static_cast<Sint16>(vertices[2].y),
			static_cast<Sint16>(vertices[3].y)
		};
		polygonRGBA(Media::Create().Renderer, vx.data(), vy.data(), 4, color.r, color.g, color.b, color.a);
	}
	postdraw();
}

/******************************** BoxShape *********************************/

/// <summary>矩形を描画する。</summary>
/// <param name="position">矩形の中心の座標</param>
/// <remarks>内部は塗り潰さない。</remarks>
void BoxShape::Draw(const Vector2<double>& position) const
{
	predraw();
	if (angle == 0.e0 || angle == M_PI) {
		//SDL_SetRenderDrawColor(Media::Create().Renderer, color.r, color.g, color.b, color.a);
		//SDL_Rect rect = { position.x - GetSize().x / 2, position.y - GetSize().y / 2, GetSize().x, GetSize().y };
		//SDL_RenderFillRect(Media::Create().Renderer, &rect);
		boxRGBA(Media::Create().Renderer,
			static_cast<Sint16>(position.x + GetSize().x / 2),
			static_cast<Sint16>(position.y - GetSize().y / 2),
			static_cast<Sint16>(position.x - GetSize().x / 2),
			static_cast<Sint16>(position.y + GetSize().y / 2),
			color.r, color.g, color.b, color.a);
	} else {
		std::array<Vector2<double>, 4> vertices = {
			position + (-GetSize() / 2).Rotate(angle),
			position + (Vector2<double>{ -GetSize().x, GetSize().y } / 2).Rotate(angle),
			position + (GetSize() / 2).Rotate(angle),
			position + (Vector2<double>{ GetSize().x, -GetSize().y } / 2).Rotate(angle)
		};
		std::array<Sint16, 4> vx = {
			static_cast<Sint16>(vertices[0].x),
			static_cast<Sint16>(vertices[1].x),
			static_cast<Sint16>(vertices[2].x),
			static_cast<Sint16>(vertices[3].x)
		};
		std::array<Sint16, 4> vy = {
			static_cast<Sint16>(vertices[0].y),
			static_cast<Sint16>(vertices[1].y),
			static_cast<Sint16>(vertices[2].y),
			static_cast<Sint16>(vertices[3].y)
		};
		filledPolygonRGBA(Media::Create().Renderer, vx.data(), vy.data(), 4, color.r, color.g, color.b, color.a);
	}
	postdraw();
}