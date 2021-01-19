// std includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// ffmpeg includes
#include "libavcodec/avcodec.h" // provides a generic encoding/decoding framework
#include "libavcodec/avfft.h"


int main(){


	int flag;
	int frameFinished;

	int frame_index = 0;
	int value = 0;

	AVPacket *pAVPacket;

	pAVPacket = (AVPacket*)av_malloc(sizeof(AVPacket));
	av_init_packet(pAVPacket);

	avcodec_register_all();


	return 0;
}
