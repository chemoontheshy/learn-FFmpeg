#include "sdl_player.h"
#include <iostream>

vsnc::vsdl::SDLPlayer::SDLPlayer(const int width, const int height, const Codec& codec, const std::string& title) :
	m_iWidth(width), m_iHeight(height),m_eCodec(codec)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Could not initialize SDL- %s\n", SDL_GetError());
		exit(-1);
	}
	// 1.创建窗口
	m_pWindow = SDL_CreateWindow(title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_iWidth,
		m_iHeight,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!m_pWindow) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window by SDL\n");
		exit(-1);
	}
	// 硬解码
	if (m_eCodec == Codec::HARDWARE) {
		// 使用支持NV12 像素格式的OpenGL渲染器
		SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
		// 硬件加速 SDL_RENDERER_ACCELERATED
		m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);
		m_u32Pixformat = SDL_PIXELFORMAT_NV12;
	}
	// 软解码
	else {
		m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
		m_u32Pixformat = SDL_PIXELFORMAT_IYUV;
	}
	// 创建纹理
	if (!m_pRenderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create Renderer by SDL\n");
		exit(-1);
	}
	m_pTexture = SDL_CreateTexture(m_pRenderer,
		m_u32Pixformat,
		SDL_TEXTUREACCESS_STREAMING,
		m_iWidth, m_iHeight);
	m_sRect.x = 0;
	m_sRect.y = 0;
	m_sRect.w = m_iWidth;
	m_sRect.h = m_iHeight;
}

vsnc::vsdl::SDLPlayer::~SDLPlayer()
{
	//销毁
	if (m_pWindow) {
		SDL_DestroyWindow(m_pWindow);
	}
	if (m_pRenderer) {
		SDL_DestroyRenderer(m_pRenderer);
	}
	if (m_pTexture) {
		SDL_DestroyTexture(m_pTexture);
	}
}


bool vsnc::vsdl::SDLPlayer::Show(const std::vector<Packet>& packets)
{
	if (m_eCodec == Codec::HARDWARE) {
		SDL_UpdateNVTexture(m_pTexture, nullptr,
			packets.at(0).data, packets.at(0).len,
			packets.at(1).data, packets.at(1).len);
	}
	else {
		SDL_UpdateYUVTexture(m_pTexture, nullptr,
			packets.at(0).data, packets.at(0).len,
			packets.at(1).data, packets.at(1).len,
			packets.at(2).data, packets.at(2).len);
	}
	
	SDL_RenderClear(m_pRenderer);
	SDL_RenderCopy(m_pRenderer, m_pTexture, nullptr, &m_sRect);
	SDL_RenderPresent(m_pRenderer);
	return false;
}
