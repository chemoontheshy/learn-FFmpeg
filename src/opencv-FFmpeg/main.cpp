/**
 * @File main.cpp
 * @Brief FFmpeg ���룬opencv����
 * @Author xzf (xzfandzgx@gmal.com)
 * @Contact
 * @Version 1.0
 * @Date 2021-01-14
 * @copyright Copyright (c) 2022
 *
 */

// 1.C��
// 2.C++ϵͳ��
#include <iostream>
// Windows
// 3.1�������C��
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <libavutil/imgutils.h>
}
// 3.2�������C++��
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>



int main()
{
	// ֡����
	int              num = 0;
	// ffmpeg����ֵ
	int              ret;
	// ��ʾ����һ֡���
	int              frameFinish;
	// ��Ƶ���
	int              videoWidth;
	// ��Ƶ�߶�
	int              videoHeight;
	// ��Ƶ������
	int              videoStreamIndex;
	// ��Ƶ����ַ
	std::string      filePath;
	// ����Packet �洢ѹ�������ݣ���Ƶ��ӦH.264���������ݣ���Ƶ��ӦPCM�������ݣ�
	AVPacket* packet;
	// ����Frame �洢��ѹ�������ݣ���Ƶ��ӦRGB/YUV�������ݡ���Ƶ��ӦPCM�������ݣ������������ĸ�ʽ
	AVFrame* frame;
	// ����Frame �洢��ѹ�������ݣ���Ƶ��ӦRGB/YUV�������ݡ���Ƶ��ӦPCM�������ݣ�Ҫ��ʾ�ĸ�ʽ
	AVFrame* frameBGR;
	// ͳ��ȫ�ֵĻ����ṹ�塣��Ҫ���ڴ����װ��ʽ��FLV/RMVB��)��
	AVFormatContext* pFormatCtx;
	// ��Ƶ������
	AVCodecContext* videoCodec;
	// ��Ƶ������
	AVCodec* videoDecoder;
	// ��Ƶ��
	AVStream* videoStream;

	//����·��
	filePath = "../../3rdparty/video/test.mp4";

	// 1.FFmpeg
	avformat_network_init();
	// 1.1.��ʼ���ܾ��
	pFormatCtx = avformat_alloc_context();

	///2017.8.5---lizhen
	AVDictionary* avdic = nullptr;
	//�ڴ�����ǰָ�����ֲ�������:̽��ʱ��/��ʱʱ��/�����ʱ��
    //���û����С,1080p�ɽ�ֵ����
	av_dict_set(&avdic, "buffer_size", "8192000", 0);
	//��tcp��ʽ��,�����udp��ʽ�򿪽�tcp�滻Ϊudp
	av_dict_set(&avdic, "rtsp_transport", "tcp", 0);
	//���ó�ʱ�Ͽ�����ʱ��,��λ΢��,3000000��ʾ3��
	av_dict_set(&avdic, "stimeout", "3000000", 0);
	//�������ʱ��,��λ΢��,1000000��ʾ1��
	av_dict_set(&avdic, "max_delay", "1000000", 0);
	//�Զ������߳���
	av_dict_set(&avdic, "threads", "auto", 0);
	// 1.2.����Ƶ
	ret = avformat_open_input(&pFormatCtx, filePath.c_str(), nullptr, &avdic);
	if (ret < 0) {
		std::cout << "Open file fail." << std::endl;
		return 0;
	}

	// 1.3.��ȡ����Ϣ
	ret = avformat_find_stream_info(pFormatCtx, nullptr);
	if (ret < 0) {
		std::cout << "Find stream info fail." << std::endl;
		return 0;
	}
	// 1.4.��Ƶ������������Ƶ��
	videoStreamIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &videoDecoder, 0);
	if (videoStreamIndex < 0) {
		std::cout << "Don't find videoStream" << std::endl;
		return 0;
	}

	// 1.5.��ȡ��Ƶ��
	videoStream = pFormatCtx->streams[videoStreamIndex];

	// 1.6.��ȡ��Ƶ������������ָ��������
	// ����ֱ��ָ����Ҳ���Ը�����������ȷ������������
	videoCodec = avcodec_alloc_context3(nullptr);
	// ��������û��ָ�������԰������Ľ��������ķŵ�videoCodec��
	avcodec_parameters_to_context(videoCodec, videoStream->codecpar);

	// 1.7.����.ѡ������뻹��Ӳ����
	videoDecoder = avcodec_find_decoder(videoCodec->codec_id);
	// Ӳ���� 
	//videoDecoder = avcodec_find_decoder_by_name("h264_qsv");
	if (!videoDecoder) {
		std::cout << "Video decoder not fond." << std::endl;
		return 0;
	}

	// 1.8.�򿪽�����
	ret = avcodec_open2(videoCodec, videoDecoder, nullptr);
	if (ret < 0) {
		std::cout << "Open videocode error." << std::endl;
		return 0;
	}

	// 1.9.�����ڴ�
	packet = av_packet_alloc();
	frame = av_frame_alloc();
	frameBGR = av_frame_alloc();
	// 2.opencv����
	// 2.1��������
	cv::namedWindow("FFmpeg", cv::WINDOW_AUTOSIZE);
	// 2.2��ʼ��ͼƬ
	auto frameWidth = videoCodec->width;
	auto frameHeight = videoCodec->height;
	// ������Ƭ�Ļ�����
	uint8_t* pBRG = new uint8_t[static_cast<size_t>(frameWidth) * static_cast<size_t>(frameHeight) * 3 + 1];
	// ����ת����ʽ
	auto swsContext = sws_getCachedContext(nullptr,
		frameWidth, frameHeight, videoCodec->pix_fmt,
		frameWidth, frameHeight, AV_PIX_FMT_BGR24,
		SWS_BILINEAR, nullptr, nullptr, nullptr);
	if (!swsContext) {
		std::cout << "sws_getCachedContext failed!" << std::endl;
		return -1;
	}
	// ���ת��ʽ���frameBRG
	av_image_fill_arrays(frameBGR->data, frameBGR->linesize,
		pBRG, AV_PIX_FMT_BGR24, frameWidth, frameHeight, 1);
	cv::Mat img(frameHeight, frameWidth, CV_8UC3, pBRG);
	// ��ʼ����
	while (true) {
		// 1.10.��pFormatCtx��ȡpacket
		if (av_read_frame(pFormatCtx, packet) < 0) {
			break;
		}
		// 1.11.ֻ������Ƶ�������
		if (packet->stream_index == videoStreamIndex) {
			// 1.12.����packet��videoCodec
			frameFinish = avcodec_send_packet(videoCodec, packet);
			if (frameFinish < 0) continue;
			// 1.13.��videoCodec��ȡ����frame
			frameFinish = avcodec_receive_frame(videoCodec, frame);
			if (frameFinish < 0) continue;
			if (frameFinish >= 0) {
				//�����ȡframe,����ͨ��ת��ʽ����qt,SDL��opencv������
				num++;
				std::cout << "finish decode " << num << " frame." << std::endl;
				//ת��
				sws_scale(swsContext, frame->data, frame->linesize, 0,
					frameHeight, frameBGR->data, frameBGR->linesize);
				cv::imshow("FFmpeg", img);
				cv::waitKey(40);
			}
		}
		av_packet_unref(packet);
		av_freep(packet);
	}
	// 1.14.�ͷ��ڴ�
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
	// 2.����
	cv::destroyWindow("FFmpeg");
	// ɾ��new������
	delete[] pBRG;
	return 0;
}