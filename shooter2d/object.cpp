#include "object.h"

using namespace Shooter;

Player::Player(const std::string &path, const float highSpeed, const float lowSpeed)
	: Object(path)
	, highSpeed(highSpeed)
	, lowSpeed(lowSpeed)
{
	for (int i = 0; i < 5; i++)
		clips[i] = { i * ImgWidth, 0, ImgWidth, ImgHeight };
}

void Player::Draw()
{
	SDL_Rect *currentClip = &clips[(Time->GetCountedFrames() / 6) % 5];
	SDL_Rect renderClip = { static_cast<int>(position.x), static_cast<int>(position.y), currentClip->w, currentClip->h };
	SDL_RenderCopy(Renderer, texture, currentClip, &renderClip);
}

void Player::Update()
{
	float speed;  // 単位：ドット毎秒
	const SDL_Keymod modStates = SDL_GetModState();
	if (modStates & KMOD_SHIFT)
		speed = lowSpeed * Timer::FPS;
	else
		speed = highSpeed * Timer::FPS;
	velocity = { 0.0f, 0.0f };
	const Uint8 *currentKeyStates = SDL_GetKeyboardState(nullptr);
	if (currentKeyStates[SDL_SCANCODE_LEFT])
		velocity.x = -speed;
	if (currentKeyStates[SDL_SCANCODE_RIGHT])
		velocity.x = +speed;
	if (currentKeyStates[SDL_SCANCODE_UP])
		velocity.y = -speed;
	if (currentKeyStates[SDL_SCANCODE_DOWN])
		velocity.y = +speed;
	if (velocity.x != 0.0f && velocity.y != 0.0f)
		velocity = velocity.Normalize() * speed;
	Move();
}

void Player::Move()
{
	position += velocity * Time->GetDeltaTime();
	if ((position.x < 0) || (position.x + ImgWidth > ScreenWidth))
		position.x -= velocity.x * Time->GetDeltaTime();
	if ((position.y < 0) || (position.y + ImgHeight > ScreenHeight))
		position.y -= velocity.y * Time->GetDeltaTime();
}