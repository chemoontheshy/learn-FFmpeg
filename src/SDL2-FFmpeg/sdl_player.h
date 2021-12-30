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
			/// <summary>SDL�Ĵ���</summary>
			SDL_Window* m_pWindow;

			/// <summary>SDL��λ��</summary>
			SDL_Rect m_sRect;

			/// <summary>SDL��ʾ��ʽ</summary>
			Uint32 m_u32Pixformat;

			/// <summary>SDL��Ⱦ��</summary>
			SDL_Renderer* m_pRenderer;

			/// <summary>SDL����</summary>
			SDL_Texture* m_pTexture;

			/// <summary>SDL�¼�</summary>
			SDL_Event    m_eEvent;

			/// <summary>SDL��ʾ�Ŀ��</summary>
			int          m_iWidth;

			/// <summary>SDL��ʾ�ĸ߶�</summary>
			int          m_iHeight;
		};
	}
}

#endif // !__SDL_PLAYER_H__


