#ifndef __SPLITLIB_H_
#define __SPLITLIB_H_
#include "utils/vsnc_utils.h"
#include <vector>

namespace vsnc
{
	namespace vnal
	{
		/// <summary>
		/// 使用Nalu替代Packet
		/// </summary>
		using Nalu = vsnc::stucture::Packet;

		/// <summary>
		/// 解析H264/HECV文件，分解成为一个个nalu包
		/// </summary>
		class SplitNalu
		{
		public:
			/// <summary>
			/// 构造函数
			/// </summary>
			/// <param name="filename">H264/HECV文件文件地址</param>
			SplitNalu(const std::string& __VSNC_IN  filename);

			/// <summary>
			/// 析构函数
			/// </summary>
			~SplitNalu() noexcept;


			/// <summary>
			/// 检查Nalu头
			/// </summary>
			/// <param name="nalu">Nalu包</param>
			/// <returns>如果一帧开头是返回4，如果是一帧中间，返回的是3，如果没有返回0</returns>
			size_t CheckHead(const Nalu& __VSNC_IN nalu) noexcept;

			/// <summary>
			/// 获取下一个nalu包
			/// </summary>
			/// <returns>下一个nalu，如果是文件末，返回Nalu{nullptr,0}</returns>
			Nalu GetNext() noexcept;

		private:
			/// <summary>文件缓存区 </summary>
			Nalu   m_iFileBuffer;
			/// <summary>开辟的文件缓存区 </summary>
			std::vector<uint8_t> m_lstFileBuffer;
		};

	}
}


#endif // !__SPLITLIB_H_