/**
  date: 2019/01/26
*/
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstdlib>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

const int ScreenWidth = 640;
const int ScreenHeight = 480;
const int ScreenFPS = 60;
const int ScreenTicksPerFrame = 1000 / ScreenFPS;

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
		{ 0, 0, 32, 48 },
		{32, 0, 32, 48},
		{64, 0, 32, 48},
		{96, 0, 32, 48},
		{128, 0, 32, 48}
	};
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
	// イベント処理用
	bool quit = false;
	SDL_Event e;
	// FPS計算用
	unsigned countedFrames = 0;
	Uint32 startTicksFPS = SDL_GetTicks(), startTicksCap;
	std::stringstream timeText;
	while (!quit) {
		startTicksCap = SDL_GetTicks();

		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_QUIT) {
				quit = true;
			} else if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_q:
					quit = true;
					break;
				}
			}
		}

		// FPSを算出・表示
		float averageOfFPS = countedFrames / ((SDL_GetTicks() - startTicksFPS) / 1000.0f);
		averageOfFPS = (averageOfFPS > 2000000) ? 0 : averageOfFPS;
		timeText.str("");
		timeText << "Average frames Per Second " << std::fixed << std::setprecision(3) << averageOfFPS;
		timeText << ", #Counted frames " << countedFrames;
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

		// 実際の描画
		SDL_RenderClear(renderer);
		SDL_Rect *currentClip = &clips[(countedFrames / 6) % 5];
		SDL_Rect renderQuadClip = { (ScreenWidth - 32) / 2, (ScreenHeight - 48) / 2, 32, 48 };
		SDL_RenderCopy(renderer, texture, currentClip, &renderQuadClip);
		SDL_RenderCopy(renderer, timerTexture, nullptr, &renderQuadText);
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