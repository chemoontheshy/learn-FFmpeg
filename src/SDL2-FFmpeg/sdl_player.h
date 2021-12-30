#ifndef __SDL_PLAYER_H__
#define __SDL_PLAYER_H__

#include <string>
#include "SDL2/SDL.h"
#include "structurer.h"
#include <vector>

using vsnc::utils::Codec;

namespace vsnc
{
	namespace vsdl
	{
		/// <summary>
		/// 数据包
		/// </summary>
		struct Packet
		{
			/// <summary>头指针</summary>
			uint8_t* data;
			/// <summary>长度</summary>
			int len;
		};

		class SDLPlayer
		{
		public:
			/// <summary>
			/// 构造函数
			/// </summary>
			/// <param name="width">显示的宽度</param>
			/// <param name="height">显示的高度</param>
			/// <param name="codec">软解码还是硬解码</param>
			/// <param name="title">SDL窗口名称</param>
			SDLPlayer(const int width, const int height,const Codec& codec,const std::string& title);

			/// <summary>
			/// 析构函数
			/// </summary>
			~SDLPlayer();

			/// <summary>
			/// 显示
			/// </summary>
			/// <param name="packets">数据包组</param>
			/// <returns>正确返回true，错误返回false</returns>
			bool Show(const std::vector<Packet>& packets);

			
		private:
			/// <summary>SDL的窗口</summary>
			SDL_Window*   m_pWindow;

			/// <summary>SDL的位置</summary>
			SDL_Rect      m_sRect;

			/// <summary>SDL显示格式</summary>
			Uint32        m_u32Pixformat;

			/// <summary>SDL渲染器</summary>
			SDL_Renderer* m_pRenderer;

			/// <summary>SDL纹理</summary>
			SDL_Texture*  m_pTexture;

			/// <summary>SDL事件</summary>
			SDL_Event     m_eEvent;

			/// <summary>SDL显示的宽度</summary>
			int           m_iWidth;

			/// <summary>SDL显示的高度</summary>
			int           m_iHeight;

			/// <summary>解码类型</summary>
			Codec         m_eCodec;

			/// <summary>解码类型</summary>
			std::string   m_sTitle;
		};
	}
}

#endif // !__SDL_PLAYER_H__


