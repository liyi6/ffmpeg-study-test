extern "C" 
{
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

#include "stdlib.h"
#include "SDL2/SDL.h"

int main(int agrc, char* agrv[]) {
	
	/* �������Եĵ�Ӱ�ļ����˴�����һ��mp4�ļ�*/
	const char* file_path = "D:/movie/silence.mp4";

	/**ע�����з�װ��/���װ��/Э��
	 * av_register_all�˺���λ��libavformat��,
	 * ��ffmpeg4.1�б��Ϊdeprecated�����ܺ�������ʹ��
	 * ����ע�����е�muxer��demuxer��protocols 
	 */
	av_register_all();

	/**����AVFormatContext����װ�����Ķ���
	 * avformat_alloc_context�˺���λ��libavformat��
	 */
	AVFormatContext* avformatContext = avformat_alloc_context();
	if (!avformatContext) {
		printf("avformat_alloc_context fail.");
		return -1;
	}

	/**�򿪲���ȡ�ļ���װ����Ϣ
	 * avformat_open_inputͨ��������ý���ļ�������ͷ��Ϣ�������ĸ������ݣ�����
	 * �����ļ�������CODEC����Ϣ��������Щ��Ϣ��䵽AVFormatContext�ṹ����
	 */
	if (avformat_open_input(&avformatContext, file_path, NULL, NULL)) {
		printf("avformat_open_input file %s fail", file_path);
		return -2;
	}

	/**��ȡ��Ҫ��CODEC��Ϣ
	 * �κ�һ�ֶ�ý���ʽ���������ṩ����Ϣ�������޵ģ����Ҳ�ͬ�Ķ�ý�����������ͷ��Ϣ������Ҳ������ͬ��
	 * ��������ý���ļ���ʱ�����������һЩ����Ҳ����˵������ͨ��avformat_open_input�����ܱ�֤�ܹ���ȡ����Ҫ����Ϣ��
	 * ����һ��Ҫʹ��avformat_find_stream_info��ȡ��Ҫ��CODEC���������õ�avformatContext->streams[i]->codec��AVCodecContext��
	 */
	if (avformat_find_stream_info(avformatContext, NULL) < 0) {
		printf("avformat_find_stream_info fail.");
		return -3;
	}

	/**���ҵ�һ����Ƶ�� 
	 * ǰ����api���ļ��е�����Ϣ�Ѿ���������AVFormatContext��streams��AVStream����
	 * AVStream
	 */
	int videoStreamIndex = -1;
	for (int i = 0; i < avformatContext->nb_streams; i++) {
		//��ʽһ�� ͨ��AVStream->AVCodecContext->AVMediaType���жϸ����Ƿ�Ϊ��Ƶ��
		//AVStream->AVCodecContext�˷��������Ϊattribute_deprecated��Ҳ�����Ժ󲻻�
		//��ʹ��
		//if (avformatContext->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) { 
		//	videoStreamIndex = i;
		//	break;
		//}

		//��ʽ����ͨ��CodecPar����ȡ
		if (avformatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
			videoStreamIndex = i;
			break;
		}
	}

	if (-1 == videoStreamIndex) {
		printf("find no video stream in the file");
		return -4;
	}


	printf("--------------- File Information ----------------\n");
	av_dump_format(avformatContext, videoStreamIndex, file_path, 0);
	printf("-------------------------------------------------\n");

	//avcodec_register_all();

	AVCodec* codec = avcodec_find_decoder(avformatContext->streams[videoStreamIndex]->codecpar->codec_id);
	if (!codec) {
		printf("Cant find codec.");
		return -5;
	}

	//AVCodecContext* codecContext = avcodec_alloc_context3(codec);
	AVCodecContext* codecContext = avformatContext->streams[videoStreamIndex]->codec;
	if (avcodec_open2(codecContext, codec, NULL)) {
		printf("Open codec fail.");
		return -6;
	}

	AVFrame* frame = av_frame_alloc();
	AVFrame* yuvFrame = av_frame_alloc();
	
	/* ����һ֡ͼƬ�Ĵ�С */
	int frame_buffer_size = av_image_get_buffer_size(codecContext->pix_fmt, codecContext->width, codecContext->height, 1);
	if (frame_buffer_size < 0) {
		printf("Open codec fail.");
		return -6;
	}
	/* ����һ֡��С��buffer */
	unsigned char* frame_buffer = (unsigned char *)av_malloc(frame_buffer_size);

	/* ���ݺ��ĸ��������ǰ�������ݣ� ���������Ҫ��AVFrame�ṹ���data��linesize����α�����ͬ���ظ�ʽ�����ڴ沼�ֵ� */
	av_image_fill_arrays(yuvFrame->data, yuvFrame->linesize, frame_buffer, codecContext->pix_fmt, codecContext->width, codecContext->height, 1);

	/* ����AVPacket�������ڶ�ȡһ֡δ��������� */
	AVPacket* packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	/* ����һ��SwsContext�����ģ���������ָʾsws_scale���������������ݣ�ת������Ҫ�ĸ�ʽ */
	SwsContext* img_convert_ctx = sws_getContext(codecContext->width, codecContext->height, codecContext->pix_fmt, codecContext->width, codecContext->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	if (!img_convert_ctx) {
		printf("sws_getContext  fail.");
		return -7;
	}
	
	/* ��ʼ��SDL�� */
	if (SDL_Init(SDL_INIT_VIDEO)) {
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return -8;
	}

	/* ����SDL���� */
	SDL_Window* window = SDL_CreateWindow("Simplest ffmpeg player's Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, codecContext->width, codecContext->height, SDL_WINDOW_OPENGL);
	if (!window) {
		printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
		return -9;
	}

	/* ������Ⱦ�� */
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

	/* �������� */
	SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, codecContext->width, codecContext->height);

	/* ������Ҫ�ڴ�������Ⱦ������ */
	SDL_Rect rect{ 0, 0, codecContext->width, codecContext->height};

	/* ��ʼ��ȡ֡�������� */
	while (av_read_frame(avformatContext, packet) >= 0){  // ���ļ��ж�ȡһ֡���ݷ���AVPacket��
		if (packet->stream_index == videoStreamIndex){   // �����֡�����ǵ�һ����Ƶ�������ݣ���ʼ����
			/* ����packet������frame�У�got_picture��0��ʾ�пɽ�������ݣ�����ֵС��0��ʾ�������*/
			int got_picture = 0;
			int ret = avcodec_decode_video2(codecContext, frame, &got_picture, packet);
			if (ret < 0){
				printf("Decode Error.\n");
				return -10;
			}

			if (got_picture){
				/* ��������ͼ��ת����YUV420P planarͼ�� */
				sws_scale(img_convert_ctx, (const unsigned char* const*)frame->data, frame->linesize, 0, codecContext->height, yuvFrame->data, yuvFrame->linesize);

				/* ��Ⱦ��ͼ�� */
				SDL_UpdateYUVTexture(texture, &rect, yuvFrame->data[0], yuvFrame->linesize[0], yuvFrame->data[1], yuvFrame->linesize[1], yuvFrame->data[2], yuvFrame->linesize[2]);
				SDL_RenderClear(renderer);
				SDL_RenderCopy(renderer, texture, NULL, &rect);
				SDL_RenderPresent(renderer);

				/* �ӳ�40s���ȡ��������һ֡�� ��֡��Լ25*/
				SDL_Delay(40);
			}
		}
		av_free_packet(packet);
	}

	sws_freeContext(img_convert_ctx);
	


	getchar();

	return  0;
}