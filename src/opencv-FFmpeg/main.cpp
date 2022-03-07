/**
 * @File main.cpp
 * @Brief FFmpeg 解码，opencv播放
 * @Author xzf (xzfandzgx@gmal.com)
 * @Contact
 * @Version 1.0
 * @Date 2021-01-14
 * @copyright Copyright (c) 2022
 *
 */

// 1.C库
// 2.C++系统库
#include <iostream>
// Windows
// 3.1其他库的C库
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <libavutil/imgutils.h>
}
// 3.2其他库的C++库
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>



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
	// 缓存Frame 存储非压缩的数据（视频对应RGB/YUV像素数据。音频对应PCM采样数据）解码器出来的格式
	AVFrame* frame;
	// 缓存Frame 存储非压缩的数据（视频对应RGB/YUV像素数据。音频对应PCM采样数据）要显示的格式
	AVFrame* frameBGR;
	// 统领全局的基本结构体。主要用于处理封装格式（FLV/RMVB等)。
	AVFormatContext* pFormatCtx;
	// 视频解码句柄
	AVCodecContext* videoCodec;
	// 视频解码器
	AVCodec* videoDecoder;
	// 视频流
	AVStream* videoStream;

	//设置路径
	filePath = "../../3rdparty/video/test.mp4";

	// 1.FFmpeg
	avformat_network_init();
	// 1.1.初始化总句柄
	pFormatCtx = avformat_alloc_context();

	///2017.8.5---lizhen
	AVDictionary* avdic = nullptr;
	//在打开码流前指定各种参数比如:探测时间/超时时间/最大延时等
    //设置缓存大小,1080p可将值调大
	av_dict_set(&avdic, "buffer_size", "8192000", 0);
	//以tcp方式打开,如果以udp方式打开将tcp替换为udp
	av_dict_set(&avdic, "rtsp_transport", "tcp", 0);
	//设置超时断开连接时间,单位微秒,3000000表示3秒
	av_dict_set(&avdic, "stimeout", "3000000", 0);
	//设置最大时延,单位微秒,1000000表示1秒
	av_dict_set(&avdic, "max_delay", "1000000", 0);
	//自动开启线程数
	av_dict_set(&avdic, "threads", "auto", 0);
	// 1.2.打开视频
	ret = avformat_open_input(&pFormatCtx, filePath.c_str(), nullptr, &avdic);
	if (ret < 0) {
		std::cout << "Open file fail." << std::endl;
		return 0;
	}

	// 1.3.获取流信息
	ret = avformat_find_stream_info(pFormatCtx, nullptr);
	if (ret < 0) {
		std::cout << "Find stream info fail." << std::endl;
		return 0;
	}
	// 1.4.视频流索引（仅视频流
	videoStreamIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &videoDecoder, 0);
	if (videoStreamIndex < 0) {
		std::cout << "Don't find videoStream" << std::endl;
		return 0;
	}

	// 1.5.获取视频流
	videoStream = pFormatCtx->streams[videoStreamIndex];

	// 1.6.获取视频流解码器或者指定解码器
	// 可以直接指定，也可以根据上下文来确定解码器类型
	videoCodec = avcodec_alloc_context3(nullptr);
	// 由于上面没有指定，所以把上下文解析出来的放到videoCodec里
	avcodec_parameters_to_context(videoCodec, videoStream->codecpar);

	// 1.7.解码.选择软解码还是硬解码
	videoDecoder = avcodec_find_decoder(videoCodec->codec_id);
	// 硬解码 
	//videoDecoder = avcodec_find_decoder_by_name("h264_qsv");
	if (!videoDecoder) {
		std::cout << "Video decoder not fond." << std::endl;
		return 0;
	}

	// 1.8.打开解码器
	ret = avcodec_open2(videoCodec, videoDecoder, nullptr);
	if (ret < 0) {
		std::cout << "Open videocode error." << std::endl;
		return 0;
	}

	// 1.9.分配内存
	packet = av_packet_alloc();
	frame = av_frame_alloc();
	frameBGR = av_frame_alloc();
	// 2.opencv部分
	// 2.1创建窗口
	cv::namedWindow("FFmpeg", cv::WINDOW_AUTOSIZE);
	// 2.2初始化图片
	auto frameWidth = videoCodec->width;
	auto frameHeight = videoCodec->height;
	// 播放照片的缓存区
	uint8_t* pBRG = new uint8_t[static_cast<size_t>(frameWidth) * static_cast<size_t>(frameHeight) * 3 + 1];
	// 设置转化格式
	auto swsContext = sws_getCachedContext(nullptr,
		frameWidth, frameHeight, videoCodec->pix_fmt,
		frameWidth, frameHeight, AV_PIX_FMT_BGR24,
		SWS_BILINEAR, nullptr, nullptr, nullptr);
	if (!swsContext) {
		std::cout << "sws_getCachedContext failed!" << std::endl;
		return -1;
	}
	// 填充转格式后的frameBRG
	av_image_fill_arrays(frameBGR->data, frameBGR->linesize,
		pBRG, AV_PIX_FMT_BGR24, frameWidth, frameHeight, 1);
	cv::Mat img(frameHeight, frameWidth, CV_8UC3, pBRG);
	// 开始解码
	while (true) {
		// 1.10.从pFormatCtx获取packet
		if (av_read_frame(pFormatCtx, packet) < 0) {
			break;
		}
		// 1.11.只有是视频流才输出
		if (packet->stream_index == videoStreamIndex) {
			// 1.12.发送packet到videoCodec
			frameFinish = avcodec_send_packet(videoCodec, packet);
			if (frameFinish < 0) continue;
			// 1.13.从videoCodec获取返回frame
			frameFinish = avcodec_receive_frame(videoCodec, frame);
			if (frameFinish < 0) continue;
			if (frameFinish >= 0) {
				//这里获取frame,可以通过转格式，用qt,SDL，opencv画出来
				num++;
				std::cout << "finish decode " << num << " frame." << std::endl;
				//转换
				sws_scale(swsContext, frame->data, frame->linesize, 0,
					frameHeight, frameBGR->data, frameBGR->linesize);
				cv::imshow("FFmpeg", img);
				cv::waitKey(40);
			}
		}
		av_packet_unref(packet);
		av_freep(packet);
	}
	// 1.14.释放内存
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
	if (swsContext) {
		sws_freeContext(swsContext);
	}
	// 2.销毁
	cv::destroyWindow("FFmpeg");
	// 删除new的数组
	delete[] pBRG;
	return 0;
}