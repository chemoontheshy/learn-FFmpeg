/**
 * @File main.cpp
 * @Brief FFmpeg 解码，RTMP推流，未经测试
 * @Author xzf (xzfandzgx@gmal.com)
 * @Contact
 * @Version 1.0
 * @Date 2021-01-14
 * @copyright Copyright (c) 2022
 *
 */
#include "splitnalu.h"
#include <Windows.h>
#include <vector>
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


int main()
{
	vsnc::vnal::Parser parser("../../3rdparty/video/test.h264");
	bool flag = true;
	std::vector<vsnc::vnal::Nalu> lstNalu;
	while (flag)
	{

		auto nalu = parser.GetNextNalu();
		if (nalu.Length <= 0)
		{
			flag = !flag;
			break;
		}
		lstNalu.push_back(nalu);
	}
	std::cout << "frame: " << lstNalu.size() << std::endl;
	// 0.FFmpeg解码 
	// 获取视频流解码器或
	AVCodecParserContext* avParser = nullptr;
	// 寻找解码器
	AVCodec* videoDecoder = nullptr;
	videoDecoder = avcodec_find_decoder(AV_CODEC_ID_H264);

	if (videoDecoder == nullptr) {
		std::cout << "video decoder not foud." << std::endl;
		return -1;
	}
	avParser = av_parser_init(videoDecoder->id);
	if (!avParser) {
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
	videoCodec->width = 640;
	videoCodec->height = 480;

	int ret = avcodec_open2(videoCodec, videoDecoder, nullptr);
	if (ret < 0) {
		std::cout << "open video codec error" << std::endl;
		return -1;
	}

	AVPacket* packet = av_packet_alloc();
	AVFrame* frame = av_frame_alloc();
	// 1.初始化网络
	avformat_network_init();
	// 2.创建输出上下文 avformat_alloc_output_context2();
	//输出的地址
	const char* outUrl = "rtmp://localhost/live/stream";
	AVFormatContext* oFormatContext = nullptr;
	ret = avformat_alloc_output_context2(&oFormatContext, nullptr, "flv", outUrl);
	if (ret < 0) {
		std::cout << "avformat_alloc_output_context2 failed." << std::endl;
		return -1;
	}
	// 3.配置输出流 av_codec_parameters_copy
	auto outStream = avformat_new_stream(oFormatContext, videoDecoder);
	outStream->time_base = AVRational{ 25,1 };
	if (!outStream) {
		std::cout << "outStream failed." << std::endl;
		return -1;
	}
	if (oFormatContext->oformat->flags & AVFMT_GLOBALHEADER) {
		std::cout << "need"<< std::endl;
		videoCodec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}
	avcodec_parameters_from_context(outStream->codecpar, videoCodec);
	av_dump_format(oFormatContext, 0, outUrl, 1);
	// 4.打开输出IO avio_open
	ret = avio_open(&oFormatContext->pb, outUrl, AVIO_FLAG_READ_WRITE);
	if (ret < 0) {
		char buf[1024];
		//获取错误信息
		av_strerror(ret, buf, sizeof(buf));
		std::cout << " failed! " << buf << std::endl;
		return -1;
	}
	std::cout << "test"<< std::endl;
	// 5.写入头部信息 avformat_write_header
	ret = avformat_write_header(oFormatContext, 0);
	if (ret < 0) {
		std::cout << "write_header failed" << std::endl;
		return -1;
	}
	// 输出
	int frameFinish = 0;
	int num = 0;
	for (auto& nalu : lstNalu) {
		auto data = nalu.Head;
		auto len = nalu.Length;
		while (nalu.Length > 0)
		{
			ret = av_parser_parse2(avParser, videoCodec, &(packet->data), &(packet->size),
				nalu.Head, static_cast<int>(nalu.Length),
				AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
			nalu.Head += ret;
			nalu.Length -= ret;
			if (ret < 0) {
				std::cout << "Error while parsing" << std::endl;
				break;
			}
			if (packet->size > 0) {
				num++;
				ret = av_interleaved_write_frame(oFormatContext, packet);
				if (ret < 0) {
					break;
				}
				std::cout << num << std::endl;
				Sleep(40);
			}
		}
		av_packet_unref(packet);
		av_freep(packet);
	}
	// 释放指针
	avcodec_close(videoCodec);
	av_frame_free(&frame);
	return 0;
}

