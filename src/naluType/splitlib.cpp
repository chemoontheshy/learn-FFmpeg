#include "splitlib.h"
#include <fstream>

vsnc::vnal::SplitNalu::SplitNalu(const std::string& __VSNC_IN filename)
{

	// �Զ����Ƹ�ʽ��
	std::ifstream file(filename, std::ios::in | std::ios::binary);
	if (!file.is_open())
	{
		std::cout << "can't open the file:" + filename << std::endl;
		exit(-1);
	}
	// ��ָ���ƶ����ļ������
	file.seekg(0, file.end);

	// ��ȡ�ļ��ĳ���
	m_iFileBuffer.Length = file.tellg();
	m_lstFileBuffer.reserve(m_iFileBuffer.Length);
	m_iFileBuffer.Data = m_lstFileBuffer.data();
	std::cout << "total:" << m_iFileBuffer.Length << std::endl;
	
	// ��ָ���ƶ����ļ��ʼ
	file.seekg(0, std::ios::beg);

	//���ļ�д����������
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
	//����Ƿ����µ�Nalu
	if (m_iFileBuffer.Length == 0)return nalu;
	auto size = __find_next_startcode(m_iFileBuffer);
	//���һ��naluʱ��ȫ��д�롣
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
	//�������ƶ�
	m_iFileBuffer.Data += size;
	m_iFileBuffer.Length -= size;
	return nalu;
}
