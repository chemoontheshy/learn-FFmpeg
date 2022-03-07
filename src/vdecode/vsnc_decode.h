#pragma once
#include <iostream>

#define __VSNC_IN
#define __VSNC_OUT
#define __VSNC_IN_OUT

//ǰ������
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
		/// ���ݰ�
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
		/// ������
		/// </summary>
		class VDecode
		{
		public:
			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="url">RTSP��ַ</param>
			VDecode(const std::string& url);

			/// <summary>
			/// ��������
			/// </summary>
			~VDecode();

			/// <summary>
			/// ��ȡһ֡���ݣ�ֻ�з���true��ʱ�������ȷ������
			/// </summary>
			/// <param name="packet">���ݰ�</param>
			/// <returns></returns>
			bool GetPacket(Packet& __VSNC_IN_OUT packet);

			bool Convert(const BasicPacket& __VSNC_IN inPacket, BasicPacket& __VSNC_OUT outPacket) noexcept;

			/// <summary>
			/// ����һ֡�Ļ���ռ��С
			/// </summary>
			/// <param name="packet">�������ݰ�</param>
			/// <returns>����һ֡����true,δ�ɹ�����false</returns>
			bool GetFrameByte(BasicPacket& __VSNC_IN_OUT packet) noexcept;
		private:
			/// <summary>
			/// ��ʼ��
			/// </summary>
			void __init();
	

		private:
			/// <summary>���</summary>
			int                      m_iWidth;
			/// <summary>�߶�</summary>
			int                      m_iHeight;
			/// <summary>��Ƶ������</summary>
			int                      m_iStreamIndex;
			/// <summary>RTSP</summary>
			std::string              m_sUrl;
			/// <summary>���ò�����ֵ��</summary>
			AVDictionary*            m_pAvdic;
			/// <summary>����Packet �洢ѹ�������ݣ���Ƶ��ӦH.264���������ݣ���Ƶ��ӦPCM�������ݣ�</summary>
			AVPacket*                m_pPacket;
			/// <summary>����Frame �洢��ѹ�������ݣ���Ƶ��ӦRGB/YUV�������ݡ���Ƶ��ӦPCM�������ݣ������������ĸ�ʽ</summary>
			AVFrame*                 m_pFrame;
			/// <summary>ͳ��ȫ�ֵĻ����ṹ�塣��Ҫ���ڴ����װ��ʽ��FLV/RMVB��)</summary>
			AVFormatContext*         m_pFormatCtx;
			/// <summary>��Ƶ������</summary>
			AVCodecContext*          m_pCodecCtx;
			/// <summary>��Ƶ������</summary>
			AVCodec*                 m_pCodec;
			/// <summary>��Ƶ��</summary>
			AVStream*                m_pStream;
		};
	}
}