// std includes
#include <libavutil/frame.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ffmpeg includes

#include "libavcodec/avcodec.h"
#include "libavcodec/avfft.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavutil/opt.h"
#include "libavutil/common.h"
#include "libavutil/channel_layout.h"
#include "libavutil/imgutils.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "libavutil/time.h"
#include "libavutil/opt.h"
#include "libavutil/pixdesc.h"
#include "libavutil/file.h"
#include "libswscale/swscale.h"

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

}

int init_output_file(){


}

int capture_video_frames(){

}


int main(){

	logging("Startup...");
	init_screen();
	logging("Startup output file...");
	init_output_file();
	logging("Capture screen");
	capture_video_frames();

	logging("Program finished");

	return 0;
}
