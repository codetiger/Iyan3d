
#ifndef VIDEO_H_
#define VIDEO_H_

#include "common.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_AV_CONFIG_H
#undef HAVE_AV_CONFIG_H
#endif

extern "C" {
	#include "libavcodec/avcodec.h"
	#include "libavutil/channel_layout.h"
	#include "libavutil/common.h"
	#include "libavutil/imgutils.h"
	#include "libavutil/mathematics.h"
	#include "libavutil/samplefmt.h"
	#include <libswscale/swscale.h>
}

bool video_encode(unsigned width, unsigned height, const char *fileName) {
    printf("Video encoding\n");
	avcodec_register_all();
    AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_MPEG1VIDEO);
    // AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "codec not found\n");
        return false;
    }

    AVCodecContext *c = avcodec_alloc_context3(codec);
    /* put sample parameters */
    c->bit_rate = 400000;
    c->width = width;
    c->height = height;
    c->time_base= (AVRational){1,24};
    c->gop_size = 10; /* emit one intra frame every ten frames */
    c->max_b_frames=1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    // c = avcodec_alloc_context3(codec);
    // c->bit_rate = 400000;
    // c->width = width;                                        // resolution must be a multiple of two (1280x720),(1900x1080),(720x480)
    // c->height = height;
    // c->time_base.num = 1;                                   // framerate numerator
    // c->time_base.den = 24;                                  // framerate denominator
    // c->gop_size = 10;                                       // emit one intra frame every ten frames
    // c->max_b_frames = 1;                                    // maximum number of b-frames between non b-frames
    // c->keyint_min = 1;                                      // minimum GOP size
    // c->i_quant_factor = (float)0.71;                        // qscale factor between P and I frames
    // c->b_frame_strategy = 20;                               ///// find out exactly what this does
    // c->qcompress = (float)0.6;                              ///// find out exactly what this does
    // c->qmin = 20;                                           // minimum quantizer
    // c->qmax = 51;                                           // maximum quantizer
    // c->max_qdiff = 4;                                       // maximum quantizer difference between frames
    // c->refs = 4;                                            // number of reference frames
    // c->trellis = 1;                                         // trellis RD Quantization
    // c->pix_fmt = PIX_FMT_YUV420P;                           // universal pixel format for video encoding
    // c->codec_id = AV_CODEC_ID_H264;
    // c->codec_type = AVMEDIA_TYPE_VIDEO;

    if (avcodec_open2(c, codec, NULL) < 0) {
        fprintf(stderr, "could not open codec\n");
        return false;
    }

    FILE *f = fopen(fileName, "wb");
    if (!f) {
        fprintf(stderr, "could not open %s\n", fileName);
        return false;
    }

    AVFrame *picture = av_frame_alloc();
    int ret = av_image_alloc(picture->data, picture->linesize, c->width, c->height, c->pix_fmt, 32);
    if (ret < 0) {
        fprintf(stderr, "could not alloc raw picture buffer\n");
        return false;
    }

	SwsContext *context = sws_getContext(c->width, c->height, AV_PIX_FMT_RGBA, c->width, c->height, AV_PIX_FMT_YUV420P, 0, 0, 0, 0);

    picture->format = c->pix_fmt;
    picture->width  = c->width;
    picture->height = c->height;

    AVPacket pkt;
	int frame = 1;
	string fn = to_string(frame);
	struct stat buffer;   
	int got_output;

	while(stat((fn + "_render.png").c_str(), &buffer) == 0) {
		printf("Encoding Frame: %d\n", frame-1);

        av_init_packet(&pkt);
        pkt.data = NULL;
        pkt.size = 0;
        fflush(stdout);

		vector<unsigned char> image;
		const char* path = (fn + "_render.png").c_str();
		unsigned error = lodepng::decode(image, width, height, path);
		if(error)
			printf("decoder error %d : %s : %s\n", error, lodepng_error_text(error), path);
		unsigned char *pngData = new unsigned char[width * height * 4];
		copy(image.begin(), image.end(), pngData);
		image.clear();


		uint8_t *inData[1]     = { pngData };
		int      inLinesize[1] = { 4 * (int)width };
		sws_scale(context, inData, inLinesize, 0, c->height, picture->data, picture->linesize);

        picture->pts = frame-1;
        
        ret = avcodec_encode_video2(c, &pkt, picture, &got_output);
        if (ret < 0) {
            fprintf(stderr, "error encoding frame\n");
            return false;
        }

        if (got_output) {
            printf("encoding frame %3d (size=%5d)\n", frame-1, pkt.size);
            fwrite(pkt.data, 1, pkt.size, f);
            av_packet_unref(&pkt);
        }

		frame++;
		fn = to_string(frame);
	}

	fflush(stdout);
	ret = avcodec_encode_video2(c, &pkt, NULL, &got_output);
	if (ret < 0) {
		fprintf(stderr, "error encoding frame\n");
		return false;
	}

	if (got_output) {
		printf("encoding frame %3d (size=%5d)\n", frame, pkt.size);
		fwrite(pkt.data, 1, pkt.size, f);
		av_packet_unref(&pkt);
	}

	uint8_t endcode[] = { 0, 0, 1, 0xb7 };
    fwrite(endcode, 1, sizeof(endcode), f);
    fclose(f);

    avcodec_close(c);
    av_free(c);
    av_freep(&picture->data[0]);
    av_frame_free(&picture);
    return true;
}

#endif
