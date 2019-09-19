#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

#include "FrameProvider.h"
#include "AlgorithmType.h"
#include "PlatformParameters.h"
#include "PandoraSingleton.h"


using namespace std;
using namespace pandora;

AVCodecContext *pCodecCtx;
AVFormatContext *pFormatCtx;
int videoStream;

extern mutex mtx;
extern condition_variable cv;
extern int ready;
extern FrameInfo frame;

FrameProvider::FrameProvider()
{
}
FrameProvider::~FrameProvider()
{
}

int FrameProvider::Init()
{
    AVCodec *pCodec;
    const char *filename = "../test/test.mp4";

    av_register_all();

    pFormatCtx = avformat_alloc_context();
    if (pFormatCtx == NULL) {
        printf("fail avformat_alloc_context\n");
    }

    if (avformat_open_input(&pFormatCtx, filename, NULL, NULL) != 0) {
        printf("av open input file failed!\n");
        exit(1);
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("av find stream info failed!\n");
        exit(1);
    }

    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }

    if (videoStream == -1) {
        printf ("find video stream failed!\n");
        exit (1);
    }

    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
     
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
     
    if (pCodec == NULL) {
        printf ("avcode find decoder failed!\n");
        exit (1);
    }

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf ("avcode open failed!\n");
        exit (1);
    }

    return 0;
}

void FrameProvider::startProviderThread()
{
    cout << "start thread ******test******" << endl << endl;

    AVFrame *pFrame;
    AVPacket *packet;
    int frameFinished = 0;
    int channels = 3;
    pFrame = av_frame_alloc();
    packet=(AVPacket *)av_malloc(sizeof(AVPacket));
    unique_lock<std::mutex> lock(mtx);
    void *data = malloc(pCodecCtx->coded_width * pCodecCtx->coded_height * sizeof(uint8_t) * channels);
    cout << pCodecCtx->coded_width << " " << pCodecCtx->coded_height << endl;
    while (av_read_frame(pFormatCtx, packet) >= 0) {
        while (ready != PROVIDER_THREAD_RUN) {
            cv.wait(lock);
        }

        if (packet->stream_index == videoStream) {
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, packet);
            if (frameFinished) {
                process(pFrame, data);
                ready = MAIN_THREAD_RUN;
                cv.notify_all();
            }
        }
        av_free_packet(packet);
    }
    av_free(pFrame);
    free(data);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    cout << "exit thread ******test******" << endl << endl;
}

int FrameProvider::process(AVFrame* pFrame, void *data)
{
    AVFrame2NV21(data, pFrame);
    frame.frame = data;
    frame.w = pFrame->width;
    frame.h = pFrame->height;
    frame.stride = 64;
    frame.scanline = 64;
    frame.type = FRAME_TYPE_SNAPSHOT;
    frame.format = FRAME_FORMAT_YUV_420_NV21;

    return 0;
}
 
int FrameProvider::AVFrame2yuv420(void *data, AVFrame* frame)
{
    int i, j, k;
    for (i = 0; i < frame->height; i++) {
        memcpy(data + frame->width * i,
            frame->data[0] + frame->linesize[0] * i,
            frame->width);
    }
    for (j = 0; j < frame->height / 2; j++) {
        memcpy(data + frame->width * i + frame->width / 2 * j,
            frame->data[1] + frame->linesize[1] * j,
            frame->width / 2);
    }
    for (k = 0; k < frame->height / 2; k++) {
        memcpy(data + frame->width * i + frame->width / 2 * j + frame->width / 2 * k,
            frame->data[2] + frame->linesize[2] * k,
            frame->width / 2);
    }
    return 0;
}

int FrameProvider::AVFrame2NV21(void *data, AVFrame* frame)
{
    cout << frame->linesize[0] << " " << frame->linesize[1] << " " << frame->linesize[2] << endl;
    int i, j, k;
    uint8_t *buf2 = (uint8_t *)data;

    for (i = 0; i < frame->height; i++) {
        memcpy(data + frame->width * i,
            frame->data[0] + frame->linesize[0] * i,
            frame->width);
    }

    uint8_t *buf = (uint8_t *)(data + frame->width * i);
    for (j = 0, k = 0; j < frame->height * frame->width / 2; j+=2, k++) {
        *(buf + j) = *(frame->data[2] + k);
        *(buf + j + 1) = *(frame->data[1] + k);
    }
    return 0;
}

