#ifndef __UNPACK_H_
#define __UNPACK_H_
#include <iostream>

namespace vsnc
{
	namespace vpack
	{
		/// <summary>编码协议枚举</summary>
		enum class CodecID : int { H264, HEVC };
		/// <summary>
		/// 码流包
		/// </summary>
		typedef struct __VPacket
		{
			/// <summary>头指针</summary>
			uint8_t* Head;
			/// <summary>码流长度</summary>
			int      Size;
			/// <summary>时间戳</summary>
			uint32_t  Timestamp;

			int16_t  seq;

			int      mark;
		} __VPacket;

		__VPacket unPack(const CodecID& codec_id, uint8_t* const stream, const int size);
	}
}

#endif