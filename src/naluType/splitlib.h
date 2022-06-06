#ifndef __SPLITLIB_H_
#define __SPLITLIB_H_
#include "utils/vsnc_utils.h"
#include <vector>

namespace vsnc
{
	namespace vnal
	{
		/// <summary>
		/// ʹ��Nalu���Packet
		/// </summary>
		using Nalu = vsnc::stucture::Packet;

		/// <summary>
		/// ����H264/HECV�ļ����ֽ��Ϊһ����nalu��
		/// </summary>
		class SplitNalu
		{
		public:
			/// <summary>
			/// ���캯��
			/// </summary>
			/// <param name="filename">H264/HECV�ļ��ļ���ַ</param>
			SplitNalu(const std::string& __VSNC_IN  filename);

			/// <summary>
			/// ��������
			/// </summary>
			~SplitNalu() noexcept;


			/// <summary>
			/// ���Naluͷ
			/// </summary>
			/// <param name="nalu">Nalu��</param>
			/// <returns>���һ֡��ͷ�Ƿ���4�������һ֡�м䣬���ص���3�����û�з���0</returns>
			size_t CheckHead(const Nalu& __VSNC_IN nalu) noexcept;

			/// <summary>
			/// ��ȡ��һ��nalu��
			/// </summary>
			/// <returns>��һ��nalu��������ļ�ĩ������Nalu{nullptr,0}</returns>
			Nalu GetNext() noexcept;

		private:
			/// <summary>�ļ������� </summary>
			Nalu   m_iFileBuffer;
			/// <summary>���ٵ��ļ������� </summary>
			std::vector<uint8_t> m_lstFileBuffer;
		};

	}
}


#endif // !__SPLITLIB_H_