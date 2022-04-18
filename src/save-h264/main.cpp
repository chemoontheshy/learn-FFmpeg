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
#include <fstream>
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
	AVFormatContext* pFormatCtx;
	// ��Ƶ������
	AVCodecContext* videoCodec;
	// ��Ƶ������
	AVCodec* videoDecoder;
	// ��Ƶ��
	AVStream* videoStream;

	//����·��
	filePath = "../../3rdparty/video/test.mp4";

	// 1.��ʼ���ܾ��
	pFormatCtx = avformat_alloc_context();

	// 2.����Ƶ
	ret = avformat_open_input(&pFormatCtx, filePath.c_str(), nullptr, nullptr);
	if (ret < 0) {
		std::cout << "Open file fail." << std::endl;
		return 0;
	}

	// 3.��ȡ����Ϣ
	ret = avformat_find_stream_info(pFormatCtx, nullptr);
	if (ret < 0) {
		std::cout << "Find stream info fail." << std::endl;
		return 0;
	}
	// 4.��Ƶ������������Ƶ��
	videoStreamIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &videoDecoder, 0);
	if (videoStreamIndex < 0) {
		std::cout << "Don't find videoStream" << std::endl;
		return 0;
	}

	// 5.��ȡ��Ƶ��
	videoStream = pFormatCtx->streams[videoStreamIndex];

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
	// ��ʼ����

	// ��ʼ����
	std::string path = "60s_640x380_yuv420p.h264";
	std::fstream f;
	f.open(path, std::ios::out | std::ios::binary);
	// 
	const AVBitStreamFilter* absFilter = nullptr;
	AVBSFContext* absCtx = nullptr;

	// 1.�ҵ���Ӧ�������Ĺ�����
	absFilter = av_bsf_get_by_name("h264_mp4toannexb");

	// 2.�����������ڴ�
	av_bsf_alloc(absFilter, &absCtx);

	// 3.��ӽ���������
	avcodec_parameters_copy(absCtx->par_in, videoStream->codecpar);

	// 4��ʼ��������
	av_bsf_init(absCtx);
	//�궨
	while (true) {
		// 10.��pFormatCtx��ȡpacket
		if (av_read_frame(pFormatCtx, packet) < 0) {
			break;
		}
		// 11.ֻ������Ƶ�������
		if (packet->stream_index == videoStreamIndex) {
			if (av_bsf_send_packet(absCtx, packet) < 0) continue;
			if (av_bsf_receive_packet(absCtx, packet) < 0)continue;
			f.write(reinterpret_cast<char*>(packet->data), packet->size);
		}
		av_packet_unref(packet);
		av_freep(packet);
	}
	f.close();
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
	if (pFormatCtx) {
		avformat_free_context(pFormatCtx);
	}
	return 0;
}