// std includes
#include <libavcodec/codec_id.h>
#include <libavcodec/packet.h>
#include <libavutil/dict.h>
#include <libavutil/frame.h>
#include <libavutil/mem.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ffmpeg includes

#include <libavcodec/avcodec.h>
#include <libavcodec/avfft.h>
#include <libavdevice/avdevice.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <libavutil/opt.h>
#include <libavutil/common.h>
#include <libavutil/channel_layout.h>
#include <libavutil/imgutils.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>
#include <libavutil/time.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
#include <libavutil/file.h>
#include <libswscale/swscale.h>

AVInputFormat *pAVInputFormat;
AVOutputFormat *output_format;

AVCodecContext *pAVCodecContext;

AVFormatContext *pAVFormatContext;

AVFrame *pAVFrame;
AVFrame *outFrame;

AVCodec *pAVCodec;
AVCodec *outAVCodec;

AVPacket *pAVPacket;

AVDictionary *options;

AVOutputFormat *outAVOutputFormat;
AVFormatContext *outAVFormatContext;
AVCodecContext *outAVCodecContext;

AVStream *video_st;
AVFrame *outAVFrame;

const char *dev_name;
const char *output_file;

double video_pts;

int out_size;
int codec_id;
int value;
int VideoStreamIndx;

static void logging(const char *fmt, ...){
	va_list args;
	fprintf(stderr, "LOG: ");
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
}

int init_screen(){
	value = 0;
	options = NULL;
	pAVFormatContext = NULL;
	pAVFormatContext = avformat_alloc_context();

	pAVInputFormat = av_find_input_format("x11grab"); // same as the ffmpeg command
	value = avformat_open_input(&pAVFormatContext, ":0.0+0,0", pAVInputFormat, NULL);

	if(value != 0){
		logging("Error, open input device. %d", value);
		exit(1);
	}

	value = av_dict_set(&options, "preset", "medium", 0);

	if(value != 0){
		logging("Error, setting preset values");
		exit(1);
	}

	VideoStreamIndx = -1;
	int i;
	/* Find the first video stream index */
	for(i = 0; i < pAVFormatContext->nb_streams; i++){
		if(pAVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO){
			VideoStreamIndx = i;
			break;
		}
	}

	if(VideoStreamIndx == -1){
		logging("Can't find the video stream index. (-1)");
		exit(1);
	}

	pAVCodecContext = pAVFormatContext->streams[VideoStreamIndx]->codec;

	pAVCodec = avcodec_find_decoder(pAVCodecContext->codec_id);
	if(pAVCodec == NULL){
		logging("Can't find decoder");
		exit(1);
	}

	value = avcodec_open2(pAVCodecContext, pAVCodec, NULL); // Initialize the AVCodecContext to use AVCodec
	if(value < 0){
		logging("Can't open the av codec");
		exit(1);
	}
	return 0;
}

int init_output_file(){
	outAVOutputFormat = NULL;
	value = 0;
	output_file = "./output.mp4";

	avformat_alloc_output_context2(&outAVFormatContext, NULL, NULL, output_file);
	if(!outAVFormatContext){
		logging("Error, allocating av format output context");
		exit(1);
	}

	output_format = av_guess_format(NULL, output_file, NULL);
	if(!output_format){
		logging("Error, guessing video format");
		exit(1);
	}

	video_st = avformat_new_stream(outAVFormatContext, NULL);
	if(!video_st){
		logging("Error in create av format new stream");
		exit(1);
	}

	outAVCodecContext = avcodec_alloc_context3(outAVCodec);
	if(!outAVCodecContext){
		logging("Error, allocating the codec contexts");
		exit(1);
	}

	/* set property of the video file */
	outAVCodecContext = video_st->codec;
	outAVCodecContext->codec_id = AV_CODEC_ID_MPEG4;// AV_CODEC_ID_MPEG4; // AV_CODEC_ID_H264 // AV_CODEC_ID_MPEG1VIDEO
	outAVCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
	outAVCodecContext->pix_fmt  = AV_PIX_FMT_YUV420P;
	outAVCodecContext->bit_rate = 400000; // 2500000
	outAVCodecContext->width = 1920;
	outAVCodecContext->height = 1080;
	outAVCodecContext->gop_size = 3;
	outAVCodecContext->max_b_frames = 2;
	outAVCodecContext->time_base.num = 1;
	outAVCodecContext->time_base.den = 30; // 15fps

	if(codec_id == AV_CODEC_ID_H264){
		av_opt_set(outAVCodecContext->priv_data, "preset", "slow", 0);
	}

	outAVCodec = avcodec_find_encoder(AV_CODEC_ID_MPEG4);
	if(!outAVCodec){
		logging("Error, find the av codecs");
		exit(1);
	}

	if(outAVFormatContext->oformat->flags & AVFMT_GLOBALHEADER){
		outAVCodecContext->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	}

	value = avcodec_open2(outAVCodecContext, outAVCodec, NULL);

	if(value < 0){
		logging("Error, open the avcodec");
		exit(1);
	}

	if(!(outAVFormatContext->flags & AVFMT_NOFILE)){
		if(avio_open2(&outAVFormatContext->pb, output_file, AVIO_FLAG_WRITE, NULL, NULL) < 0){
			logging("Error, creating the video file");
			exit(1);
		}
	}

	if(!outAVFormatContext->nb_streams){
		logging("Error, output file dose not contain any stream");
		exit(1);
	}

	value = avformat_write_header(outAVFormatContext, &options);
	if(value < 0){
		logging("Error, writing the header context");
		exit(1);
	}
	return 0;
}

int capture_video_frames(){
	int frameFinished;
	value = 0;

	pAVPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
	av_init_packet(pAVPacket);

	pAVFrame = av_frame_alloc();
	if(!pAVFrame){
		logging("Error, unable to release the avframe resources");
		exit(1);
	}

	outFrame = av_frame_alloc();
	if(!outFrame){
		logging("Error, unable to release the avframe resources for outframe");
		exit(1);
	}

	int nbytes = av_image_get_buffer_size(outAVCodecContext->pix_fmt, outAVCodecContext->width, outAVCodecContext->height, 32);
	uint8_t *video_outbuf = (uint8_t *)av_malloc(nbytes);
	if(video_outbuf == NULL){
		logging("Error, allocate memory");
		exit(1);
	}

	value = av_image_fill_arrays(outFrame->data, outFrame->linesize, video_outbuf, AV_PIX_FMT_YUV420P, outAVCodecContext->width, outAVCodecContext->height, 1);
	if(value == 0){
		logging("Error, in filling image array");
	}

	struct SwsContext *swsCtx_;

/*	swsCtx_ = sws_getContext(pAVCodecContext->width, pAVCodecContext->height, pAVCodecContext->pix_fmt,
							 outAVCodecContext->width, outAVCodecContext->height, outAVCodecContext->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
*/
	 swsCtx_ = sws_getContext(pAVCodecContext->width, pAVCodecContext->height, pAVCodecContext->pix_fmt, 
				   outAVCodecContext->width, outAVCodecContext->height, outAVCodecContext->pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);

	int ii = 0;
	int no_frames = 100;
	logging("No. of frames to capture: %d", no_frames);

	AVPacket outPacket;
	int j = 0;

	int got_picture;

	while(av_read_frame(pAVFormatContext, pAVPacket) >= 0){
		if(ii++ == no_frames)
			break;
		if(pAVPacket->stream_index == VideoStreamIndx){
			value = avcodec_decode_video2(pAVCodecContext, pAVFrame, &frameFinished, pAVPacket);
			if(value < 0)
				logging("Error, unable to decode video");
			if(frameFinished){// frame decoded
				sws_scale(swsCtx_, (unsigned char const **)pAVFrame->data, pAVFrame->linesize, 0, pAVCodecContext->height, outFrame->data, outFrame->linesize);
				av_init_packet(&outPacket);

				avcodec_encode_video2(outAVCodecContext, &outPacket, outFrame, &got_picture);

				if(!got_picture){
					if(outPacket.pts != AV_NOPTS_VALUE)
						outPacket.pts = av_rescale_q(outPacket.pts, video_st->time_base, video_st->time_base);
					if(outPacket.dts != AV_NOPTS_VALUE)
						outPacket.dts = av_rescale_q(outPacket.dts, video_st->time_base, video_st->time_base);

					printf("Write frame %3d (size= %2d)\n", j++, outPacket.size);
					if(av_write_frame(outAVFormatContext , &outPacket) != 0){
						logging("Error, in write video frame");
					}
					av_packet_unref(&outPacket);
				}
				av_packet_unref(&outPacket);
			}
		}
	}

	value = av_write_trailer(outAVFormatContext);
	if(value < 0){
		logging("Error writing av trailer");
		exit(1);
	}
	av_free(video_outbuf);
	return 0;
}

int free_AV(){
	avformat_close_input(&pAVFormatContext);
	if(!pAVFormatContext)
		logging("File closed");
	else
		logging("Fail to close the file");
	avformat_free_context(pAVFormatContext);
	if(!pAVFormatContext)
		logging("avformat free successfully");
	else{
		logging("Fail to free avformat context");
		exit(1);
	}
	return 0;
}


int main(){
	logging("Startup registers...");
	avdevice_register_all();
	logging("Registers started\n");

	logging("Startup screen...");
	init_screen();
	logging("Screen started.\n");

	logging("Startup output file...");
	init_output_file();
	logging("Output file started\n");

	logging("Capture screen...");
	capture_video_frames();
	logging("Program finished\n");

	free_AV();

	return 0;
}
