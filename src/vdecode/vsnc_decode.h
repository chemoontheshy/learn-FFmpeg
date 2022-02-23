#pragma once
#include <iostream>

#define __VSNC_IN
#define __VSNC_OUT
#define __VSNC_IN_OUT

//前向声明
struct AVDictionary;
struct AVPacket;
struct AVFrame;
struct AVFormatContext;
struct AVCodecContext;
struct AVCodec;
struct AVStream;
struct SwsContext;
enum AVPixelFormat;

namespace vsnc
{
	namespace vdc
	{

		

		/// <summary>
		/// 数据包
		/// </summary>
		struct Packet
		{
			uint8_t*		Data;
			size_t			Length;
		};

		struct BasicPacket : Packet
		{
			AVPixelFormat   Format;
			size_t          Width;
			size_t          Height;
		};

		/// <summary>
		/// 解码器
		/// </summary>
		class VDecode
		{
		public:
			/// <summary>
			/// 构造函数
			/// </summary>
			/// <param name="url">RTSP地址</param>
			VDecode(const std::string& url);

			/// <summary>
			/// 析构函数
			/// </summary>
			~VDecode();

			/// <summary>
			/// 获取一帧数据，只有返回true的时候才有正确的数据
			/// </summary>
			/// <param name="packet">数据包</param>
			/// <returns></returns>
			bool GetPacket(Packet& __VSNC_IN_OUT packet);

			bool Convert(const BasicPacket& __VSNC_IN inPacket, BasicPacket& __VSNC_OUT outPacket) noexcept;

			/// <summary>
			/// 计算一帧的缓存空间大小
			/// </summary>
			/// <param name="packet">基础数据包</param>
			/// <returns>解码一帧返回true,未成功返回false</returns>
			bool GetFrameByte(BasicPacket& __VSNC_IN_OUT packet) noexcept;
		private:
			/// <summary>
			/// 初始化
			/// </summary>
			void __init();
	

		private:
			/// <summary>宽度</summary>
			int                      m_iWidth;
			/// <summary>高度</summary>
			int                      m_iHeight;
			/// <summary>视频流索引</summary>
			int                      m_iStreamIndex;
			/// <summary>RTSP</summary>
			std::string              m_sUrl;
			/// <summary>配置参数键值对</summary>
			AVDictionary*            m_pAvdic;
			/// <summary>缓存Packet 存储压缩的数据（视频对应H.264等码流数据，音频对应PCM采样数据）</summary>
			AVPacket*                m_pPacket;
			/// <summary>缓存Frame 存储非压缩的数据（视频对应RGB/YUV像素数据。音频对应PCM采样数据）解码器出来的格式</summary>
			AVFrame*                 m_pFrame;
			/// <summary>统领全局的基本结构体。主要用于处理封装格式（FLV/RMVB等)</summary>
			AVFormatContext*         m_pFormatCtx;
			/// <summary>视频解码句柄</summary>
			AVCodecContext*          m_pCodecCtx;
			/// <summary>视频解码器</summary>
			AVCodec*                 m_pCodec;
			/// <summary>视频流</summary>
			AVStream*                m_pStream;
		};
	}
}