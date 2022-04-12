/**
 * @File main.cpp
 * @Brief FFmpeg ����MP4,������Ƶ
 * @Author xzf (xzfandzgx@gmal.com)
 * @Contact
 * @Version 1.0
 * @Date 2021-04-12
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
#include "libswresample/swresample.h"
}

#define MAX_AUDIO_FRAME_SIZE 192000

int main()
{

    const char inFileName[] = "../../3rdparty/video/test.mp4";
    const char outFileName[] = "test.pcm";
    FILE* file = fopen(outFileName, "w+b");
    if (!file) {
        printf("Cannot open output file.\n");
        return -1;
    }

    AVFormatContext* fmtCtx = avformat_alloc_context();
    AVCodecContext* codecCtx = NULL;
    AVPacket* pkt = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    int aStreamIndex = -1;

    do {

        if (avformat_open_input(&fmtCtx, inFileName, NULL, NULL) < 0) {
            printf("Cannot open input file.\n");
            return -1;
        }
        if (avformat_find_stream_info(fmtCtx, NULL) < 0) {
            printf("Cannot find any stream in file.\n");
            return -1;
        }

        av_dump_format(fmtCtx, 0, inFileName, 0);

        for (size_t i = 0; i < fmtCtx->nb_streams; i++) {
            if (fmtCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                aStreamIndex = (int)i;
                break;
            }
        }
        if (aStreamIndex == -1) {
            printf("Cannot find audio stream.\n");
            return -1;
        }

        AVCodecParameters* aCodecPara = fmtCtx->streams[aStreamIndex]->codecpar;
        AVCodec* codec = avcodec_find_decoder(aCodecPara->codec_id);
        if (!codec) {
            printf("Cannot find any codec for audio.\n");
            return -1;
        }
        codecCtx = avcodec_alloc_context3(codec);
        if (avcodec_parameters_to_context(codecCtx, aCodecPara) < 0) {
            printf("Cannot alloc codec context.\n");
            return -1;
        }
        codecCtx->pkt_timebase = fmtCtx->streams[aStreamIndex]->time_base;

        if (avcodec_open2(codecCtx, codec, NULL) < 0) {
            printf("Cannot open audio codec.\n");
            return -1;
        }
        size_t num = 0;
        std::fstream f;
        f.open("m_f32le.pcm", std::ios::out | std::ios::app);
        while (av_read_frame(fmtCtx, pkt) >= 0) {
            if (pkt->stream_index == aStreamIndex) {
                if (avcodec_send_packet(codecCtx, pkt) >= 0) {
                    while (avcodec_receive_frame(codecCtx, frame) >= 0) {
                        /*
                          Planar��ƽ�棩�������ݸ�ʽ���з�ʽΪ (�ر��ס���ô����Ե�nb_samples�������������������ֽڽ���:
                          LLLLLLRRRRRRLLLLLLRRRRRRLLLLLLRRRRRRL...��ÿ��LLLLLLRRRRRRΪһ����Ƶ֡��
                          ������P�����ݸ�ʽ�����������У����з�ʽΪ��
                          LRLRLRLRLRLRLRLRLRLRLRLRLRLRLRLRLRLRL...��ÿ��LRΪһ����Ƶ������
                        */
                        if (av_sample_fmt_is_planar(codecCtx->sample_fmt)) {
                            int numBytes = av_get_bytes_per_sample(codecCtx->sample_fmt);
                            //pcm����ʱ��LRLRLR��ʽ������Ҫ����������
                          
                            for (int i = 0; i < frame->nb_samples; i++) {
                                for (int ch = 0; ch < codecCtx->channels; ch++) {
                                    fwrite((char*)frame->data[ch] + numBytes * i, 1, numBytes, file);
                                    f.write((char*)frame->data[ch] + numBytes * i, numBytes);
                                }
                            }
                            std::cout << "finish decode " << num++ << " frame." << std::endl;
                        }
                    }
                }
            }
            av_packet_unref(pkt);
        }
        f.close();
    } while (0);
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_close(codecCtx);
    avcodec_free_context(&codecCtx);
    avformat_free_context(fmtCtx);

    fclose(file);

    return 0;
}