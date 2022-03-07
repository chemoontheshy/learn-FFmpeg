#include "vsnc_decode.h"
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

vsnc::vdc::VDecode::VDecode(const std::string& url) :
	m_iWidth(0),
	m_iHeight(0),
	m_iStreamIndex(-1),
	m_sUrl(url),
	m_pAvdic(nullptr),
	m_pPacket(av_packet_alloc()),
	m_pFrame(av_frame_alloc()),
	m_pFormatCtx(avformat_alloc_context()),
	m_pCodecCtx(avcodec_alloc_context3(nullptr)),
	m_pCodec(nullptr),
	m_pStream(nullptr)
{
	__init();
}

vsnc::vdc::VDecode::~VDecode()
{
	// 1.14.�ͷ��ڴ�
	if (m_pPacket) {
		av_packet_unref(m_pPacket);
	}
	if (m_pFrame) {
		av_frame_free(&m_pFrame);
	}
	if (m_pCodecCtx) {
		avcodec_close(m_pCodecCtx);
	}
	if (m_pFormatCtx) {
		avformat_free_context(m_pFormatCtx);
	}
	if (m_pAvdic) {
		av_dict_free(&m_pAvdic);
	}
	if (m_pCodec) {
		av_free(m_pCodec);
	}
	if (m_pStream) {
		av_free(m_pStream);
	}
}

bool vsnc::vdc::VDecode::GetPacket(Packet& __VSNC_IN_OUT packet)
{
	return false;
}

bool vsnc::vdc::VDecode::Convert(const BasicPacket& __VSNC_IN inPacket, BasicPacket& __VSNC_OUT outPacket) noexcept
{
	return false;
}

bool vsnc::vdc::VDecode::GetFrameByte(BasicPacket& __VSNC_IN_OUT packet) noexcept
{
	return false;
}

void vsnc::vdc::VDecode::__init()
{
	//1.��������
	avformat_network_init();
	//2.RTSP����
	//�ڴ�����ǰָ�����ֲ�������:̽��ʱ��/��ʱʱ��/�����ʱ��
	//���û����С,1080p�ɽ�ֵ����
	av_dict_set(&m_pAvdic, "buffer_size", "8192000", 0);
	//��tcp��ʽ��,�����udp��ʽ�򿪽�tcp�滻Ϊudp
	av_dict_set(&m_pAvdic, "rtsp_transport", "tcp", 0);
	//���ó�ʱ�Ͽ�����ʱ��,��λ΢��,3000000��ʾ3��
	av_dict_set(&m_pAvdic, "stimeout", "3000000", 0);
	//�������ʱ��,��λ΢��,1000000��ʾ1��
	av_dict_set(&m_pAvdic, "max_delay", "1000000", 0);
	//�Զ������߳���
	av_dict_set(&m_pAvdic, "threads", "auto", 0);
	if (!avformat_open_input(&m_pFormatCtx, m_sUrl.c_str(), nullptr, &m_pAvdic)) {
		std::cout << "Open file fail." << std::endl;
		exit(-1);
	}
	// 1.3.��ȡ����Ϣ
	if (!avformat_find_stream_info(m_pFormatCtx, nullptr)) {
		std::cout << "Find stream info fail." << std::endl;
		exit(-1);
	}
	// 1.4.��Ƶ������������Ƶ��
	m_iStreamIndex = av_find_best_stream(m_pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &m_pCodec, 0);
	if (m_iStreamIndex < 0) {
		std::cout << "Don't find videoStream" << std::endl;
		exit(-1);
	}
	// 1.5.��ȡ��Ƶ��
	m_pStream = m_pFormatCtx->streams[m_iStreamIndex];
	// 1.6.��ȡ��Ƶ������������ָ��������
	// ����ֱ��ָ����Ҳ���Ը�����������ȷ������������
	// ��������û��ָ�������԰������Ľ��������ķŵ�videoCodec��
	avcodec_parameters_to_context(m_pCodecCtx, m_pStream->codecpar);
	// 1.7.����.ѡ������뻹��Ӳ����
	m_pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
	// Ӳ���� 
	//videoDecoder = avcodec_find_decoder_by_name("h264_qsv");
	if (!m_pCodec) {
		std::cout << "Video decoder not fond." << std::endl;
		exit(-1);
	}
	if (!avcodec_open2(m_pCodecCtx, m_pCodec, nullptr)) {
		std::cout << "Open videocode error." << std::endl;
		exit(-1);
	}
	m_iWidth = m_pCodecCtx->width;
	m_iHeight = m_pCodecCtx->height;

	std::cout << "init" << std::endl;
}
