#include <iostream>
#include <vector>

/// <summary> 编解码协议枚举</summary>
enum class CodecID : int { H264, HEVC };


/// <summary>
/// 地址
/// </summary>
typedef struct Address
{
	/// <summary>IP或者主机名</summary>
	std::string IPorHost;
	/// <summary>端口</summary>
	uint16_t    Port;
} Address;

int main(int argc, char* argv[])
{
	if (argc < 5) {
		std::cout << "params error.: H264 NONE NONE NONE 192.168.0.31 8002" << std::endl;
		return -1;
	}
	std::cout << argc << std::endl;
	std::vector<CodecID> lstCodecID;
	std::vector<Address> lstAddress;
	Address address;
	for (size_t i = 1; i < 5; i++) {
		//std::cout << argv[i] << std::endl;
		if (!strcmp(argv[i], "H264")) {
			lstCodecID.push_back(CodecID::H264);
		}
		if (!strcmp(argv[i], "HEVC")) {
			lstCodecID.push_back(CodecID::HEVC);
		}
	}
	if (argc > 5) {
		for (size_t i = 5; i < argc; i += 2) {
			std::cout << argv[i] << std::endl;
			address.IPorHost = argv[i];
			address.Port = std::atoi(argv[i + 1]);
			lstAddress.push_back(address);
		}
	}
	for (auto& codecID : lstCodecID) {
		if (codecID == CodecID::H264) {
			std::cout << "CodecID::H264" << std::endl;
		}
		if (codecID == CodecID::HEVC) {
			std::cout << "CodecID::HEVC" << std::endl;
		}
	}
	for (auto& taddress : lstAddress) {
		std::cout << taddress.IPorHost << std::endl;
		std::cout << taddress.Port << std::endl;
	}
	return 0;
}