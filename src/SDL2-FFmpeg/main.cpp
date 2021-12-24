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

class Dog
{
public:
	Dog(const int age) :m_iAge(age) {};

private:
	int m_iAge;

};

int main(int argc, char* argv[])
{
	std::vector<Dog*> lstDog;
	lstDog.push_back(new Dog(1));
	lstDog.push_back(new Dog(2));
	lstDog.push_back(new Dog(3));
	lstDog.push_back(new Dog(4));

	for (auto& dog : lstDog) {
		delete dog;
	}
	
	return 0;
}