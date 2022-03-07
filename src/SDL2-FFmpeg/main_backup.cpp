/**
 * @File main.cpp
 * @Brief FFmpeg 解码，SDL播放(未解决SDL可移动）
 * @Author xzf (xzfandzgx@gmal.com)
 * @Contact
 * @Version 1.0
 * @Date 2021-01-14
 * @copyright Copyright (c) 2022
 *
 */

#include <iostream>
#include <Windows.h>
 // Windows
extern "C"
{
	//
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}
#include "sdl_player.h"
#include <thread>


int main(int argc, char* argv[])
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
	AVFormatContext* pFormatCtx;
	// 视频解码句柄
	AVCodecContext* videoCodec;
	// 视频解码器
	AVCodec* videoDecoder;
	// 视频流
	AVStream* videoStream;
	// 解码类型---这里非常重要，要切换修改这里
	vsnc::utils::Codec codec = vsnc::utils::Codec::SOFTWARE;
	
	// 如果需要解析网络RTSP流。需要加
	// avformat_network_init();

	//设置路径
	filePath = "../../3rdparty/video/test.mp4";

	// 1.初始化总句柄
	pFormatCtx = avformat_alloc_context();

	// 2.打开视频
	ret = avformat_open_input(&pFormatCtx, filePath.c_str(), nullptr, nullptr);
	if (ret < 0) {
		std::cout << "Open file fail." << std::endl;
		return 0;
	}

	// 3.获取流信息
	ret = avformat_find_stream_info(pFormatCtx, nullptr);
	if (ret < 0) {
		std::cout << "Find stream info fail." << std::endl;
		return 0;
	}
	// 4.视频流索引（仅视频流
	videoStreamIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &videoDecoder, 0);
	if (!videoStreamIndex) {
		std::cout << "Don't find videoStream" << std::endl;
		return 0;
	}

	// 5.获取视频流
	videoStream = pFormatCtx->streams[videoStreamIndex];

	// 6.获取视频流解码器或者指定解码器
	// 可以直接指定，也可以根据上下文来确定解码器类型
	videoCodec = avcodec_alloc_context3(nullptr);
	// 由于上面没有指定，所以把上下文解析出来的放到videoCodec里
	avcodec_parameters_to_context(videoCodec, videoStream->codecpar);

	// 7.解码.选择软解码还是硬解码
	// 软解码
	if (codec == vsnc::utils::Codec::SOFTWARE) {
		videoDecoder = avcodec_find_decoder(videoCodec->codec_id);
	}
	// 硬解码 
	else {
		videoDecoder = avcodec_find_decoder_by_name("h264_qsv");
	}
	if (!videoDecoder) {
		std::cout << "Video decoder not fond." << std::endl;
		return 0;
	}

	//设置加速解码
	videoCodec->lowres = videoDecoder->max_lowres;
	videoCodec->flags2 |= AV_CODEC_FLAG2_FAST;

	// 8.打开解码器
	ret = avcodec_open2(videoCodec, videoDecoder, nullptr);
	if (ret < 0) {
		std::cout << "Open videocode error." << std::endl;
		return 0;
	}

	// 9.分配内存
	packet = av_packet_alloc();
	frame = av_frame_alloc();
	// 开始解码
	videoWidth = videoCodec->width;
	videoHeight = videoCodec->height;

	//创建SDL
	std::shared_ptr<vsnc::vsdl::SDLPlayer> sdlPlayer;
	sdlPlayer = std::make_shared<vsnc::vsdl::SDLPlayer>(videoWidth, videoHeight, codec, "SDLTest");
	//std::thread t(moveTest, sdlPlayer);
	std::vector<vsnc::vsdl::Packet> sdlPackets(3);
	while (true) {
		// 10.从pFormatCtx获取packet
		if (av_read_frame(pFormatCtx, packet) < 0) {
			break;
		}
		// 11.只有是视频流才输出
		if (packet->stream_index == videoStreamIndex) {
			// 12.发送packet到videoCodec
			frameFinish = avcodec_send_packet(videoCodec, packet);
			if (frameFinish < 0) continue;
			// 13.从videoCodec获取返回frame
			frameFinish = avcodec_receive_frame(videoCodec, frame);
			if (frameFinish < 0) continue;
			if (frameFinish >= 0) {
				//这里获取frame,可以通过转格式，用qt,SDL，opencv画出来
				num++;
				std::cout << "finish decode " << num << " frame." << std::endl;
				// 软解码
				if (codec == vsnc::utils::Codec::SOFTWARE) {
					sdlPackets.at(0).data = frame->data[0];
					sdlPackets.at(0).len = frame->linesize[0];
					sdlPackets.at(1).data = frame->data[1];
					sdlPackets.at(1).len = frame->linesize[1];
					sdlPackets.at(2).data = frame->data[2];
					sdlPackets.at(2).len = frame->linesize[2];
				}
				else {
					sdlPackets.at(0).data = frame->data[0];
					sdlPackets.at(0).len = frame->linesize[0];
					sdlPackets.at(1).data = frame->data[1];
					sdlPackets.at(1).len = frame->linesize[1];
				}

				sdlPlayer->Show(sdlPackets);
				//视频是25帧，每次间隔是40ms,实时流不需要延时，解码完就播放
				Sleep(35);
			}
		}
		av_packet_unref(packet);
		av_freep(packet);
		
	}
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
	if (pFormatCtx) {
		avformat_free_context(pFormatCtx);
	}
	return 0;
}