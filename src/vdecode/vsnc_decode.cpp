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
	// 1.14.释放内存
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
	//1.网络配置
	avformat_network_init();
	//2.RTSP配置
	//在打开码流前指定各种参数比如:探测时间/超时时间/最大延时等
	//设置缓存大小,1080p可将值调大
	av_dict_set(&m_pAvdic, "buffer_size", "8192000", 0);
	//以tcp方式打开,如果以udp方式打开将tcp替换为udp
	av_dict_set(&m_pAvdic, "rtsp_transport", "tcp", 0);
	//设置超时断开连接时间,单位微秒,3000000表示3秒
	av_dict_set(&m_pAvdic, "stimeout", "3000000", 0);
	//设置最大时延,单位微秒,1000000表示1秒
	av_dict_set(&m_pAvdic, "max_delay", "1000000", 0);
	//自动开启线程数
	av_dict_set(&m_pAvdic, "threads", "auto", 0);
	if (!avformat_open_input(&m_pFormatCtx, m_sUrl.c_str(), nullptr, &m_pAvdic)) {
		std::cout << "Open file fail." << std::endl;
		exit(-1);
	}
	// 1.3.获取流信息
	if (!avformat_find_stream_info(m_pFormatCtx, nullptr)) {
		std::cout << "Find stream info fail." << std::endl;
		exit(-1);
	}
	// 1.4.视频流索引（仅视频流
	m_iStreamIndex = av_find_best_stream(m_pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &m_pCodec, 0);
	if (m_iStreamIndex < 0) {
		std::cout << "Don't find videoStream" << std::endl;
		exit(-1);
	}
	// 1.5.获取视频流
	m_pStream = m_pFormatCtx->streams[m_iStreamIndex];
	// 1.6.获取视频流解码器或者指定解码器
	// 可以直接指定，也可以根据上下文来确定解码器类型
	// 由于上面没有指定，所以把上下文解析出来的放到videoCodec里
	avcodec_parameters_to_context(m_pCodecCtx, m_pStream->codecpar);
	// 1.7.解码.选择软解码还是硬解码
	m_pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
	// 硬解码 
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
