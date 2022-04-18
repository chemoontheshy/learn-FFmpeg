#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <fstream>
#include <thread>
#include "unpack.h"
#include "sdl_player.h"
//当前C++兼容C语言
extern "C"
{
	// avcodec:编解码(最重要的库)
#include <libavcodec/avcodec.h>
// avformat:封装格式处理
#include <libavformat/avformat.h>
// swscale:视频像素数据格式转换
#include <libswscale/swscale.h>
// avutil:工具库（大部分库都需要这个库的支持）
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
}

/// <summary>
/// 输入q退出
/// </summary>
/// <param name="run">是否运行的标志符</param>
static void quit(bool& run)
{
	char c = '\0';
	do {
		std::cin >> c;
	} while ('q' != c);
	run = false;
}


int main(int argc, char* argv[])
{
	
	// 3.socket部分
	// socket初始化
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return -1;
	}
	// 接受数据部分
	auto socketFD = socket(AF_INET, SOCK_DGRAM, 0);
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(4000);
	((struct sockaddr_in&)addr).sin_addr.s_addr = INADDR_ANY;
	if (bind(socketFD, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in)) < 0) {
		std::cout << "UDP::bind() failed." << std::endl;
		return -1;
	}
	// 接收数据包缓存区
	char rBuf[1500] = { 0 };
	sockaddr_in client;
	socklen_t clientLen = sizeof(sockaddr_in);

	int width = 1920;
	int height = 1088;
	// 设置两帧的socket接收的缓冲区
	int rcvBufSize = width * height * 3;
	socklen_t optLen = sizeof(rcvBufSize);
	if (setsockopt(socketFD, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*>(&rcvBufSize), optLen) < 0) {
		std::cout << "setsockopt failed." << std::endl;
		return -1;
	}

	// 4.FFmpeg解码 
	// 获取视频流解码器或
	AVCodecParserContext* parser = nullptr;
	// 寻找解码器
	AVCodec* videoDecoder = nullptr;
	videoDecoder = avcodec_find_decoder_by_name("h264_qsv");

	if (videoDecoder == nullptr) {
		std::cout << "video decoder not foud." << std::endl;
		return -1;
	}
	parser = av_parser_init(videoDecoder->id);
	if (!parser) {
		fprintf(stderr, "parser not found\n");
		return -1;
	}
	// 获取视频流解码器或者指定解码器
	auto videoCodec = avcodec_alloc_context3(videoDecoder);
	if (!videoCodec) {
		fprintf(stderr, "Could not allocate video codec context\n");
		return -1;
	}
	// 设置加速解码
	videoCodec->lowres = videoDecoder->max_lowres;
	videoCodec->flags2 |= AV_CODEC_FLAG2_FAST;
	// 初始化
	// 打开解码器
	videoCodec->width = width;
	videoCodec->height = height;

	int ret = avcodec_open2(videoCodec, videoDecoder, nullptr);
	if (ret < 0) {
		std::cout << "open video codec error" << std::endl;
		return -1;
	}

	AVPacket* packet = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();
	AVFrame* frameBGR = av_frame_alloc();
	//5
	std::unique_ptr<vsnc::vsdl::SDLPlayer> sdlPlayer;
	sdlPlayer = std::make_unique<vsnc::vsdl::SDLPlayer>(width, height, vsnc::utils::Codec::HARDWARE, "SDLTest");
	std::vector<vsnc::vsdl::Packet> sdlPackets(3);
	// 6.退出设置
	bool run = true;
	std::thread watch(&quit, std::ref(run));

	// 7.其他
	// 计数
	int num = 0;
	// 用于判断是否生成帧
	int frameFinish = 0;
	// 输出RTP解析后数据包
	vsnc::vpack::__VPacket outPacket;

	while (run) {
		auto size = recvfrom(socketFD, rBuf, sizeof(rBuf), 0, reinterpret_cast<sockaddr*>(&client), &clientLen);
		if (size > 0) {
			outPacket = vsnc::vpack::unPack(vsnc::vpack::CodecID::H264, reinterpret_cast<uint8_t*>(rBuf), size);
			while (outPacket.Size > 0)
			{
				ret = av_parser_parse2(parser, videoCodec, &(packet->data), &(packet->size),
					outPacket.Head, static_cast<int>(outPacket.Size),
					AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
				outPacket.Head += ret;
				outPacket.Size -= ret;
				if (ret < 0) {
					std::cout << "Error while parsing" << std::endl;
					break;
				}
			
				if (packet->size > 0) {
					frameFinish = avcodec_send_packet(videoCodec, packet);

					if (frameFinish < 0) {
						continue;
					}
					frameFinish = avcodec_receive_frame(videoCodec, frame);

					if (frameFinish < 0) {
						continue;
					}
					else {
						num++;
						std::cout << "finish decode " << num << " frame." << std::endl;
						// 显示：
						sdlPackets.at(0).data = frame->data[0];
						sdlPackets.at(0).len = frame->linesize[0];
						sdlPackets.at(1).data = frame->data[1];
						sdlPackets.at(1).len = frame->linesize[1];
						sdlPlayer->Show(sdlPackets);
					}
				}
			}
			av_packet_unref(packet);
			av_freep(packet);
		}

	}
	//file.close();
	// 释放指针
	avcodec_close(videoCodec);
	av_frame_free(&frame);
	av_frame_free(&frameBGR);
	// 关闭socket
	closesocket(socketFD);
	return 0;
}