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
	videoStreamIndex = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, &videoDecoder, 0);
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
	std::string path = "audio_decode_f32le.pcm";
	std::fstream f;
	f.open(path, std::ios::out);

	while (true) {
		// 10.��pFormatCtx��ȡpacket
		if (av_read_frame(pFormatCtx, packet) < 0) break;
		// 11.ֻ������Ƶ�������
		if (packet->stream_index == videoStreamIndex) 
		{
			// 12.����packet��videoCodec
			frameFinish = avcodec_send_packet(videoCodec, packet);
			if (frameFinish < 0) continue;
			// 13.��videoCodec��ȡ����frame
			frameFinish = avcodec_receive_frame(videoCodec, frame);
			if (frameFinish < 0) continue;
			if (frameFinish >= 0) {
				//�����ȡframe,����ͨ��ת��ʽ����qt,SDL��opencv������
				num++;
				std::cout << "finish decode " << num << " frame." << std::endl;
				if (av_sample_fmt_is_planar(videoCodec->sample_fmt))
				{
					//���������Ĵ�С
					int numBytes = av_get_bytes_per_sample(videoCodec->sample_fmt);
					//PCM����ʱʱLRLRLR��ʽ��������Ҫ����������
					// nb_samples��ÿ֡����������AAC��������һ��Ϊ1024
					for (size_t i = 0; i < frame->nb_samples; i++)
					{
						// channelsͨ��������ʵ������������������
						for (size_t ch = 0; ch < videoCodec->channels; ch++)
						{
							//����д�룬���磬ԭʼ��Ƶ��f32le��ͨ����f32le�Ǹ����ͣ�������32λ����3�ֽڣ�le��С�� ��
							// ��������ͨ����1024�Ĳ����㣬���ȸ�λ1024*4��
							// ��д���һ�����㣬��д����Ĳ�����
							f.write(reinterpret_cast<char*>(frame->data[ch] + numBytes * i), numBytes);
						}
					}
				}
			}
		}
		av_packet_unref(packet);
		av_freep(packet);
	}
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