#ifndef __SDL_PLAYER_H__
#define __SDL_PLAYER_H__

#include "SDL2/SDL.h"

namespace vsnc
{
	namespace vsdl
	{
		class Player
		{
		public:
			
		private:
			/// <summary>SDL的窗口</summary>
			SDL_Window* m_pWindow;

			/// <summary>SDL的位置</summary>
			SDL_Rect m_sRect;

			/// <summary>SDL显示格式</summary>
			Uint32 m_u32Pixformat;

			/// <summary>SDL渲染器</summary>
			SDL_Renderer* m_pRenderer;

			/// <summary>SDL纹理</summary>
			SDL_Texture* m_pTexture;

			/// <summary>SDL事件</summary>
			SDL_Event    m_eEvent;

			/// <summary>SDL显示的宽度</summary>
			int          m_iWidth;

			/// <summary>SDL显示的高度</summary>
			int          m_iHeight;
		};
	}
}

#endif // !__SDL_PLAYER_H__


