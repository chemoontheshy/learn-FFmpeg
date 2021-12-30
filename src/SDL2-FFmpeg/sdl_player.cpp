#include "sdl_player.h"

vsnc::vsdl::SDLPlayer::SDLPlayer(const int width, const int height, const Codec& codec, const std::string& title) :
	m_iWidth(width), m_iHeight(height),m_eCodec(codec)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize SDL- %s\n", SDL_GetError());
		exit(-1);
	}
	m_pWindow = SDL_CreateWindow()
}
