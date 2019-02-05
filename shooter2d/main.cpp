/**
  date: 2019/01/26
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

const int ScreenWidth = 640;
const int ScreenHeight = 480;
const int ScreenFPS = 60;
const int ScreenTicksPerFrame = 1000 / ScreenFPS;

const float PlayerHighSpeed = 4.0f;  // 単位：ドット毎フレーム
const float PlayerLowSpeed = 2.0f;
const int PlayerImgWidth = 32;
const int PlayerImgHeight = 48;

namespace Shooter {
	struct Vector2
	{
		float x, y;

		Vector2(float x, float y) : x(x), y(y) {}

		Vector2 &operator=(const Vector2 &vector) = default;

		//Vector2 Add(Vector2 vector)
		Vector2 operator+(const Vector2 &vector) const
		{
			return { this->x + vector.x, this->y + vector.y };
		}

		//Vector2 Subtract(Vector2 vector)
		Vector2 operator-(const Vector2 &vector) const
		{
			return { this->x - vector.x, this->y + vector.y };
		}

		//Vector2 ScalarMultiply(float scalar)
		Vector2 operator*(const float &scalar) const
		{
			return { this->x * scalar, this->y * scalar };
		}

		Vector2 &operator+=(const Vector2 &vector)
		{
			x += vector.x;
			y += vector.y;
			return *this;
		}

		float Magnitude()
		{
			return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
		}

		Vector2 Normalize()
		{
			//return vector.ScalarMultiply(1.0f / vector.Magnitude());
			return { x / Magnitude(), y / Magnitude() };
		}
	};
}

int main(int argc, char* args[])
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		std::cerr << "Warning: Linear texture filtering not enabled!" << std::endl;
	SDL_Window *window = SDL_CreateWindow("2D shooter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ScreenWidth, ScreenHeight, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	//SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	std::string path("images/Reimudot.png");
	SDL_Texture *texture = IMG_LoadTexture(renderer, path.c_str());
	if (texture == nullptr) {
		std::cerr << "Unable to load image " << path << "!  SDL_image Error: " << IMG_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	if (TTF_Init() == -1) {
		std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
		return EXIT_FAILURE;
	}
	TTF_Font *font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 14);
	SDL_Texture *timerTexture;
	if (font == nullptr) {
		std::cerr << "Failed to load lazy font! SDL_ttf Error: " << TTF_GetError() << std::endl;
		return EXIT_FAILURE;
	}

	SDL_Rect clips[5] = {
		{ 0 * PlayerImgWidth, 0, PlayerImgWidth, PlayerImgHeight },
		{ 1 * PlayerImgWidth, 0, PlayerImgWidth, PlayerImgHeight },
		{ 2 * PlayerImgWidth, 0, PlayerImgWidth, PlayerImgHeight },
		{ 3 * PlayerImgWidth, 0, PlayerImgWidth, PlayerImgHeight },
		{ 4 * PlayerImgWidth, 0, PlayerImgWidth, PlayerImgHeight }
	};
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	// イベント処理用
	bool quit = false;
	SDL_Event e;
	// FPS計算用
	unsigned countedFrames = 0;
	Uint32 startTicksFPS = SDL_GetTicks(), startTicksCap = SDL_GetTicks();
	float deltaTime;  // HACK: 前のフレームからの経過時間（秒）。将来的に下のframeTicksなどと統合したい。
	std::stringstream timeText;
	// 操作用
	using namespace Shooter;
	Vector2 playerPosition = { (ScreenWidth - PlayerImgWidth) / 2.0f, ScreenHeight - PlayerImgHeight * 1.5f };  // ここで言う位置とは自機画像の左上端のこと。
	Vector2 playerVelocity = { 0.0f, 0.0f };
	std::stringstream debugText;  // デバッグ用。適当なものを出力する。
	SDL_Texture *debugTexture;
	while (!quit) {
		deltaTime = (SDL_GetTicks() - startTicksCap) / 1000.0f;  // HACK: 最初のループだけ変な値になる？　countedFramesを使った方がいい？
		startTicksCap = SDL_GetTicks();

		while (SDL_PollEvent(&e) != 0) {
			switch (e.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
					quit = true;
				break;
			}
		}
		float playerSpeed;  // 単位：ドット毎秒
		const SDL_Keymod modStates = SDL_GetModState();
		if (modStates & KMOD_SHIFT)
			playerSpeed = PlayerLowSpeed * ScreenFPS;
		else
			playerSpeed = PlayerHighSpeed * ScreenFPS;
		playerVelocity = { 0.0f, 0.0f };
		const Uint8 *currentKeyStates = SDL_GetKeyboardState(nullptr);
		if (currentKeyStates[SDL_SCANCODE_LEFT])
			playerVelocity.x = -playerSpeed;
		if (currentKeyStates[SDL_SCANCODE_RIGHT])
			playerVelocity.x = +playerSpeed;
		if (currentKeyStates[SDL_SCANCODE_UP])
			playerVelocity.y = -playerSpeed;
		if (currentKeyStates[SDL_SCANCODE_DOWN])
			playerVelocity.y = +playerSpeed;
		if (playerVelocity.x != 0.0f && playerVelocity.y != 0.0f)
			playerVelocity = playerVelocity.Normalize() * playerSpeed;

		// FPSを算出・表示
		float averageOfFPS = std::fmod(countedFrames / ((SDL_GetTicks() - startTicksFPS) / 1000.0f), 2000000);
		timeText.str("");
		timeText << "Average frames Per Second " << std::fixed << std::setprecision(3) << averageOfFPS;
		timeText << ", #Counted frames " << countedFrames;
		timeText << ", Delta time " << std::fixed << std::setprecision(3) << deltaTime;
		int textWidth = 0 , textHeight = 0;
		{
			SDL_Color textColor = { 0xFF, 0xFF, 0xFF };
			SDL_Surface *textSurface = TTF_RenderText_Solid(font, timeText.str().c_str(), textColor);
			timerTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
			textWidth = textSurface->w;
			textHeight = textSurface->h;
			SDL_FreeSurface(textSurface);
		}
		SDL_Rect renderQuadText = { 0, ScreenHeight - textHeight, textWidth, textHeight };

		// 自機の移動
		playerPosition += playerVelocity * deltaTime;
		if ((playerPosition.x < 0) || (playerPosition.x + PlayerImgWidth > ScreenWidth))
			playerPosition.x -= playerPosition.x * deltaTime;
		if ((playerPosition.y < 0) || (playerPosition.y + PlayerImgHeight > ScreenHeight))
			playerPosition.y -= playerVelocity.y * deltaTime;

		// デバッグメッセージ
		debugText.str("");
		debugText << "Player's position (" << std::fixed << std::setprecision(0) << playerPosition.x << ", " << playerPosition.y << "), ";
		debugText << "Player's velocity (" << std::fixed << std::setprecision(0) << playerVelocity.x << ", " << playerVelocity.y << "); ";
		debugText << "Is pressed shift keys " << (modStates & KMOD_SHIFT) << ", ";
		debugText << "Speed " << playerSpeed;
		{
			SDL_Color textColor = { 0xFF, 0xFF, 0xFF };
			SDL_Surface *textSurface = TTF_RenderText_Solid(font, debugText.str().c_str(), textColor);
			debugTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
			textWidth = textSurface->w;
			textHeight = textSurface->h;
			SDL_FreeSurface(textSurface);
		}
		SDL_Rect renderQuadDebug = { 0, 0, textWidth, textHeight };

		// 実際の描画
		SDL_RenderClear(renderer);
		SDL_Rect *currentClip = &clips[(countedFrames / 6) % 5];
		SDL_Rect renderQuadClip = { static_cast<int>(playerPosition.x), static_cast<int>(playerPosition.y), currentClip->w, currentClip->h };
		SDL_RenderCopy(renderer, texture, currentClip, &renderQuadClip);
		SDL_RenderCopy(renderer, timerTexture, nullptr, &renderQuadText);
		SDL_RenderCopy(renderer, debugTexture, nullptr, &renderQuadDebug);
		SDL_RenderPresent(renderer);

		countedFrames = ++countedFrames;  // ステージを切り換えるときなどに0に戻す。
		// 垂直同期をしない場合。小数点以下が桁落ちする分、誤差が生じる。
		int frameTicks = SDL_GetTicks() - startTicksCap;
		if (frameTicks < ScreenTicksPerFrame)
			SDL_Delay(ScreenTicksPerFrame - frameTicks);
	}

	SDL_DestroyTexture(texture);
	TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}