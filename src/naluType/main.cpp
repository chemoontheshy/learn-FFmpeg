#include "splitlib.h"

enum class NaluType
{
	NALU_TYPE_NOME      = 0,
	NALU_TYPE_SLICE     = 1,
	NALU_TYPE_DPA       = 2,
	NALU_TYPE_DPB       = 3,
	NALU_TYPE_DPC       = 4,
	NALU_TYPE_IDR       = 5,
	NALU_TYPE_SEI       = 6,
	NALU_TYPE_SPS       = 7,
	NALU_TYPE_PPS       = 8,
	NALU_TYPE_AUD       = 9,
	NALU_TYPE_EOSEQ     = 10,
	NALU_TYPE_EOSTREAM  = 11,
	NALU_TYPE_FILL      = 12,
};

struct NaluHeader
{
	uint8_t nal_unit_type      : 5;
	uint8_t nal_ref_idc        : 2;
	uint8_t forbidden_zero_bit : 1;
};

int main()
{
	vsnc::vnal::SplitNalu splitNalu("../../3rdparty/video/test.h264");
	size_t num = 0;
	size_t len = 0;
	size_t startcodeLen = 0;
	while (true)
	{
		num++;
		auto nalu = splitNalu.GetNext();
		if (nalu.Length <= 0) break;
		//len += nalu.Length;

		//std::cout << nalu.Length << std::endl;
		NaluHeader* naluHeader = nullptr;

		if (splitNalu.CheckHead(nalu) == 3)
		{
			startcodeLen = 3;
			//std::cout << "--------------3----------------" << std::endl;
			naluHeader = reinterpret_cast<NaluHeader*>(nalu.Data+3);
		}
		else if (splitNalu.CheckHead(nalu) == 4)
		{
			startcodeLen = 4;
			//std::cout << "--------------4----------------" << std::endl;
			naluHeader = reinterpret_cast<NaluHeader*>(nalu.Data + 4);
		}
		else {
			std::cout << "error" << std::endl;
		}

		if (naluHeader)
		{
			//std::cout << "forbidden_zero_bit : " << static_cast<size_t>(naluHeader->forbidden_zero_bit) << std::endl;
			//std::cout << "nal_ref_idc        : " << static_cast<size_t>(naluHeader->nal_ref_idc) << std::endl;
			std::cout << num << "," << static_cast<size_t>(naluHeader->nal_unit_type) << "," << startcodeLen << "," << nalu.Length<< std::endl;
		}
	}
	//std::cout << len << std::endl;
	return 0;
}