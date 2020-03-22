#include "media.h"
#include <iostream>
#include <cstdlib>

using namespace Shooter;

Media::Media()
{
	//SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		std::cerr << "Warning: Linear texture filtering not enabled!" << std::endl;
	Window = SDL_CreateWindow("Bullet Hell 2D Shmup", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (Window == nullptr) {
		std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	//Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
	if (Renderer == nullptr) {
		std::cerr << "Renderer could not be created! SDL Error: " << SDL_GetError() << std::endl;
		exit(EXIT_FAILURE);
	}
	/*TargetTexture = SDL_CreateTexture(Renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
	if (TargetTexture == nullptr) {
		std::cerr << "Failed to create target texture! SDL Error: " << SDL_GetError() << std::endl;
	}*/
}

Media::~Media()
{
	//SDL_DestroyTexture(TargetTexture);
	SDL_DestroyRenderer(Renderer);
	SDL_DestroyWindow(Window);
	SDL_Quit();
}

void Media::Clear() const
{
	SDL_SetRenderDrawColor(Renderer, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(Renderer);
}

/// <summary>ウィンドウのサイズを変更する。</summary>
/// <param name="width">変更後の幅</param>
/// <param name="height">変更後の高さ</param>
void Media::ChangeScreenSize(const int width, const int height)
{
	if (width > 0 && height > 0) {
		this->width = width;
		this->height = height;
		SDL_SetWindowSize(Window, width, height);
	}
}

/// <summary>フルスクリーンに切り替える。</summary>
/// <remarks>フルスクリーン時は元のサイズに戻す。</remarks>
void Media::ChangeToFullScreen() const
{
	if (IsFullScreen())
		SDL_SetWindowFullscreen(Window, 0);
	else
		SDL_SetWindowFullscreen(Window, SDL_WINDOW_FULLSCREEN);
}

bool Media::IsFullScreen() const
{
	return ((SDL_GetWindowFlags(Window) & SDL_WINDOW_FULLSCREEN) != 0);
}

void Media::Present() const
{
	SDL_RenderPresent(Renderer);
}