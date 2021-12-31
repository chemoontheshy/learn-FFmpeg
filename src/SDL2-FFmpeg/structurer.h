#ifndef __STRUCTURER_H__
#define __STRUCTURER_H__

namespace vsnc
{
	namespace utils
	{
		enum class Codec : int
		{
			SOFTWARE,
			HARDWARE
		};

		struct Header
		{
			uint16_t Id;
			uint16_t Type;
			uint8_t  No[6];
			uint16_t Sequence;
		};
	}
}



#endif // !__STRUCTURER_H__
