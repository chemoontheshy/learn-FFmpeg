/**
 * @File main.cpp
 * @Brief FFmpeg ����MP4(�Ѳ���
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
	// ����Frame �洢��ѹ�������ݣ���Ƶ��ӦRGB/YUV�������ݡ���Ƶ��ӦPCM�������ݣ�
	AVFrame* frame;
	// ͳ��ȫ�ֵĻ����ṹ�塣��Ҫ���ڴ����װ��ʽ��FLV/RMVB��)��
	AVFormatContext* iFormatCtx;
	// ��Ƶ������
	AVCodecContext* videoCodec;
	// ��Ƶ������
	AVCodec* videoDecoder;
	// ��Ƶ��
	AVStream* videoStream;

	//����·��
	filePath = "../../3rdparty/video/leishen.mp4";

	// 1.��ʼ���ܾ��
	iFormatCtx = avformat_alloc_context();

	// 2.����Ƶ
	ret = avformat_open_input(&iFormatCtx, filePath.c_str(), nullptr, nullptr);
	if (ret < 0) {
		std::cout << "Open file fail." << std::endl;
		return 0;
	}

	// 3.��ȡ����Ϣ
	ret = avformat_find_stream_info(iFormatCtx, nullptr);
	if (ret < 0) {
		std::cout << "Find stream info fail." << std::endl;
		return 0;
	}
	// 4.��Ƶ������������Ƶ��
	videoStreamIndex = av_find_best_stream(iFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &videoDecoder, 0);
	if (videoStreamIndex<0) {
		std::cout << "Don't find videoStream" << std::endl;
		return 0;
	}

	// 5.��ȡ��Ƶ��
	videoStream = iFormatCtx->streams[videoStreamIndex];

	// 6.��ȡ��Ƶ������������ָ��������
	// ����ֱ��ָ����Ҳ���Ը�����������ȷ������������
	videoCodec = avcodec_alloc_context3(nullptr);
	// ��������û��ָ�������԰������Ľ��������ķŵ�videoCodec��
	avcodec_parameters_to_context(videoCodec, videoStream->codecpar);

	// 7.����.ѡ������뻹��Ӳ����
	videoDecoder = avcodec_find_decoder(videoCodec->codec_id);
	// Ӳ���� 
	//videoDecoder = avcodec_find_decoder_by_name("h264_qsv");
	if (!videoDecoder) {
		std::cout << "Video decoder not fond." << std::endl;
		return 0;
	}

	// 8.�򿪽�����
	ret = avcodec_open2(videoCodec, videoDecoder, nullptr);
	if (ret < 0) {
		std::cout << "Open videocode error." << std::endl;
		return 0;
	}

	// 9.�����ڴ�
	packet = av_packet_alloc();
	frame = av_frame_alloc();

	// ������Ƶ
	// 1.��ʼ������,�����ļ����Բ�Ҫ
	// avformat_network_init();
	// 2.������������� avformat_alloc_output_context2();
	//����ĵ�ַ
	const char* outUrl = "test20220414.flv";
	AVFormatContext* oFormatCtx = nullptr;
	ret = avformat_alloc_output_context2(&oFormatCtx, nullptr, nullptr, outUrl);
	if (ret < 0) {
		std::cout << "avformat_alloc_output_context2 failed." << std::endl;
		return -1;
	}
	// 3.��������� av_codec_parameters_copy
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
	// 4.�����IO avio_open
	ret = avio_open(&oFormatCtx->pb, outUrl, AVIO_FLAG_READ_WRITE);
	if (ret < 0) {
		char buf[1024];
		//��ȡ������Ϣ
		av_strerror(ret, buf, sizeof(buf));
		std::cout << " failed! " << buf << std::endl;
		return -1;
	}
	std::cout << "test" << std::endl;
	// 5.д��ͷ����Ϣ avformat_write_header
	ret = avformat_write_header(oFormatCtx, nullptr);
	if (ret < 0) {
		std::cout << "write_header failed" << std::endl;
		return -1;
	}
	char errorStr[1024];
	// ��ʼ����
	int64_t test = 0;
	while (true) {
		// 10.��pFormatCtx��ȡpacket
		if (av_read_frame(iFormatCtx, packet) < 0) {
			break;
		}
		// 11.ֻ������Ƶ�������
		if (packet->stream_index == videoStreamIndex) {
			// 12.����packet��videoCodec
			av_packet_rescale_ts(packet, videoStream->time_base, oFormatCtx->streams[packet->stream_index]->time_base);
			packet->pos = -1;
			ret = av_interleaved_write_frame(oFormatCtx, packet);
			std::cout << "finish" << num++ << std::endl;
		}
		av_packet_unref(packet);
		av_freep(packet);
	}
	//av_write_trailer(oFormatCtx);
	// 14.�ͷ��ڴ�
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