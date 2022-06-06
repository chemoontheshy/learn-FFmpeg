#include "splitlib.h"
#include <fstream>

vsnc::vnal::SplitNalu::SplitNalu(const std::string& __VSNC_IN filename)
{

	// 以二进制格式打开
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "can't open the file:" + filename << std::endl;
		exit(-1);
	}
	// 把指针移动到文件最后面
	file.seekg(0, file.end);

	// 获取文件的长度
	m_iFileBuffer.Length = file.tellg();
	m_lstFileBuffer.reserve(m_iFileBuffer.Length);
	m_iFileBuffer.Data = m_lstFileBuffer.data();
	std::cout << "total:" << m_iFileBuffer.Length << std::endl;
	
	// 把指针移动到文件最开始
	file.seekg(0, std::ios::beg);

	//把文件写到缓冲区里
	file.read(reinterpret_cast<char*>(m_iFileBuffer.Data), m_iFileBuffer.Length);
	
	file.close();


}

vsnc::vnal::SplitNalu::~SplitNalu() noexcept
{
}

size_t vsnc::vnal::SplitNalu::CheckHead(const Nalu& __VSNC_IN nalu) noexcept
{
	auto ptr = nalu.Data;
	if (*(nalu.Data) == 0x00 && *(nalu.Data + 1) == 0x00 && *(nalu.Data + 2) == 0x01) return 3;
	if (*(nalu.Data) == 0x00 && *(nalu.Data + 1) == 0x00 && *(nalu.Data + 2) == 0x00 && *(nalu.Data + 3) == 0x01) return 4;
	return 0;
}

vsnc::vnal::Nalu vsnc::vnal::SplitNalu::GetNext() noexcept
{
	auto __find_next_startcode = [=](const Nalu& __VSNC_IN nalu)->size_t
	{
		size_t i = 0;
		for (i; i < nalu.Length - 6; i++)
		{
			if (CheckHead(Nalu{ nalu.Data + 3 + i ,0 }) > 0) return i;
		}
		return 0;

	};
	Nalu nalu{nullptr,0};
	//检查是否有新的Nalu
	if (m_iFileBuffer.Length == 0)return nalu;
	auto size = __find_next_startcode(m_iFileBuffer);
	//最后一个nalu时把全部写入。
	if (size == 0)
	{
		nalu.Data = m_iFileBuffer.Data;
		nalu.Length = m_iFileBuffer.Length;
		m_iFileBuffer.Data = nullptr;
		m_iFileBuffer.Length = 0;
		return nalu;
	}

	size += 3;
	nalu.Data = m_iFileBuffer.Data;
	nalu.Length = size;
	//缓存区移动
	m_iFileBuffer.Data += size;
	m_iFileBuffer.Length -= size;
	return nalu;
}
