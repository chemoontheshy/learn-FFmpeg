/**
 * @File main.cpp
 * @Brief FFmpeg 解码MP4(已测试
 * @Author xzf (xzfandzgx@gmal.com)
 * @Contact
 * @Version 1.0
 * @Date 2021-01-14
 * @copyright Copyright (c) 2022
 *
 */

#include <iostream>
 // Windows
extern "C"
{
	//
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}

int main()
{
	// 帧计算
	int              num = 0;
	// ffmpeg返回值
	int              ret;
	// 表示解码一帧完成
	int              frameFinish;
	// 视频宽带
	int              videoWidth;
	// 视频高度
	int              videoHeight;
	// 视频流索引
	int              videoStreamIndex;
	// 视频流地址
	std::string      filePath;
	// 缓存Packet 存储压缩的数据（视频对应H.264等码流数据，音频对应PCM采样数据）
	AVPacket* packet;
	// 缓存Frame 存储非压缩的数据（视频对应RGB/YUV像素数据。音频对应PCM采样数据）
	AVFrame* frame;
	// 统领全局的基本结构体。主要用于处理封装格式（FLV/RMVB等)。
	AVFormatContext* iFormatCtx;
	// 视频解码句柄
	AVCodecContext* videoCodec;
	// 视频解码器
	AVCodec* videoDecoder;
	// 视频流
	AVStream* videoStream;

	//设置路径
	filePath = "../../3rdparty/video/leishen.mp4";

	// 1.初始化总句柄
	iFormatCtx = avformat_alloc_context();

	// 2.打开视频
	ret = avformat_open_input(&iFormatCtx, filePath.c_str(), nullptr, nullptr);
	if (ret < 0) {
		std::cout << "Open file fail." << std::endl;
		return 0;
	}

	// 3.获取流信息
	ret = avformat_find_stream_info(iFormatCtx, nullptr);
	if (ret < 0) {
		std::cout << "Find stream info fail." << std::endl;
		return 0;
	}
	// 4.视频流索引（仅视频流
	videoStreamIndex = av_find_best_stream(iFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &videoDecoder, 0);
	if (videoStreamIndex<0) {
		std::cout << "Don't find videoStream" << std::endl;
		return 0;
	}

	// 5.获取视频流
	videoStream = iFormatCtx->streams[videoStreamIndex];

	// 6.获取视频流解码器或者指定解码器
	// 可以直接指定，也可以根据上下文来确定解码器类型
	videoCodec = avcodec_alloc_context3(nullptr);
	// 由于上面没有指定，所以把上下文解析出来的放到videoCodec里
	avcodec_parameters_to_context(videoCodec, videoStream->codecpar);

	// 7.解码.选择软解码还是硬解码
	videoDecoder = avcodec_find_decoder(videoCodec->codec_id);
	// 硬解码 
	//videoDecoder = avcodec_find_decoder_by_name("h264_qsv");
	if (!videoDecoder) {
		std::cout << "Video decoder not fond." << std::endl;
		return 0;
	}

	// 8.打开解码器
	ret = avcodec_open2(videoCodec, videoDecoder, nullptr);
	if (ret < 0) {
		std::cout << "Open videocode error." << std::endl;
		return 0;
	}

	// 9.分配内存
	packet = av_packet_alloc();
	frame = av_frame_alloc();

	// 保存视频
	// 1.初始化网络,本地文件可以不要
	// avformat_network_init();
	// 2.创建输出上下文 avformat_alloc_output_context2();
	//输出的地址
	const char* outUrl = "test20220414.flv";
	AVFormatContext* oFormatCtx = nullptr;
	ret = avformat_alloc_output_context2(&oFormatCtx, nullptr, nullptr, outUrl);
	if (ret < 0) {
		std::cout << "avformat_alloc_output_context2 failed." << std::endl;
		return -1;
	}
	// 3.配置输出流 av_codec_parameters_copy
	auto outStream = avformat_new_stream(oFormatCtx, nullptr);
	if (!outStream) {
		std::cout << "outStream failed." << std::endl;
		return -1;
	}
	ret = avcodec_parameters_copy(outStream->codecpar, videoStream->codecpar);
	if (ret < 0) {
		std::cout << "Failed to copy codec parameters\n" << std::endl;
		return -1;
	}
	outStream->codecpar->codec_tag = 0;

	//AVStream outStream;
	av_dump_format(oFormatCtx, 0, outUrl, 1);
	// 4.打开输出IO avio_open
	ret = avio_open(&oFormatCtx->pb, outUrl, AVIO_FLAG_READ_WRITE);
	if (ret < 0) {
		char buf[1024];
		//获取错误信息
		av_strerror(ret, buf, sizeof(buf));
		std::cout << " failed! " << buf << std::endl;
		return -1;
	}
	std::cout << "test" << std::endl;
	// 5.写入头部信息 avformat_write_header
	ret = avformat_write_header(oFormatCtx, nullptr);
	if (ret < 0) {
		std::cout << "write_header failed" << std::endl;
		return -1;
	}
	char errorStr[1024];
	// 开始解码
	int64_t test = 0;
	while (true) {
		// 10.从pFormatCtx获取packet
		if (av_read_frame(iFormatCtx, packet) < 0) {
			break;
		}
		// 11.只有是视频流才输出
		if (packet->stream_index == videoStreamIndex) {
			// 12.发送packet到videoCodec
			av_packet_rescale_ts(packet, videoStream->time_base, oFormatCtx->streams[packet->stream_index]->time_base);
			packet->pos = -1;
			ret = av_interleaved_write_frame(oFormatCtx, packet);
			std::cout << "finish" << num++ << std::endl;
		}
		av_packet_unref(packet);
		av_freep(packet);
	}
	//av_write_trailer(oFormatCtx);
	// 14.释放内存
	if (packet) {
		av_packet_unref(packet);
	}
	if (frame) {
		av_frame_free(&frame);
	}
	if (videoCodec) {
		avcodec_close(videoCodec);
	}
	if (iFormatCtx) {
		avformat_free_context(iFormatCtx);
	}
	/*if (oFormatCtx) {
		avformat_free_context(oFormatCtx);
	}*/
	return 0;
}