#include <stdint.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <condition_variable>

#include "FrameGenerator.h"
#include "AlgorithmType.h"
#include "PlatformParameters.h"
#include "PandoraSingleton.h"

#define SCREEN_W 800
#define SCREEN_H 600
#define REFRESH_EVENT  (SDL_USEREVENT + 1)
#define BREAK_EVENT  (SDL_USEREVENT + 2)
#define MEDIA_PATH "../test/test.mp4"

using namespace std;
using namespace pandora;

extern FrameInfo gFrame;


int thread_exit = 0;
int preview_pause = 0;
int start_capture = 0;
int g_process_finished = 0;
extern std::mutex mtx;
extern std::condition_variable cv;

FrameGenerator::FrameGenerator():
    pCodecCtx(NULL),
    pFormatCtx(NULL),
    mFrame(NULL),
    mFrameYUV(NULL),
    mPacket(NULL),
    mWin(NULL),
    mRender(NULL),
    mTexture(NULL),
    videoStream(-1),
    out_buffer(NULL),
    img_convert_ctx(NULL)
{
}
FrameGenerator::~FrameGenerator()
{
}

int FrameGenerator::Init()
{
    AVCodec *pCodec;
    const char *filename = MEDIA_PATH;
    int pixel_w = 0;
    int pixel_h = 0;

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
    mFrame = av_frame_alloc();
    mFrameYUV = av_frame_alloc();

    out_buffer = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_NV21,  pCodecCtx->width, pCodecCtx->height, 1));
    av_image_fill_arrays(mFrameYUV->data, mFrameYUV->linesize, out_buffer, AV_PIX_FMT_NV21, pCodecCtx->width, pCodecCtx->height, 1);
#if 0
    //Output Info-----------------------------
    printf("---------------- File Information ---------------\n");
    av_dump_format(pFormatCtx,0,filename,0);
    printf("-------------------------------------------------\n");
#endif
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
        pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_NV21, SWS_BICUBIC, NULL, NULL, NULL);

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
        printf( "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }

    pixel_w = pCodecCtx->width;
    pixel_h = pCodecCtx->height;
    mWin = SDL_CreateWindow("SDL terminal 空格-->stop 回车-->capture", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_W, SCREEN_H, SDL_WINDOW_SHOWN);

    if (!mWin) {
        printf("SDL: could not SDL_CreateWindow - exiting:%s\n",SDL_GetError());
        return -1;
    }

    mRender = SDL_CreateRenderer(mWin, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (!mRender) {
        printf("SDL: could not SDL_CreateRenderer - exiting:%s\n",SDL_GetError());
        return -1;
    }

    mTexture = SDL_CreateTexture(mRender, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pixel_w, pixel_h);

    return 0;
}


int FrameGenerator::preview_refresh_thread(void *opaque)
{
    thread_exit = 0;
    preview_pause = 0;
    while (!thread_exit) {
        if (!preview_pause) {
            SDL_Event event;
            event.type = REFRESH_EVENT;
            SDL_PushEvent(&event);
        }
        SDL_Delay(10);
    }
    cv.notify_all();
    printf("preview thread exit ...\n");
    return 0;
}

int FrameGenerator::start()
{
    SDL_Thread *preview_tid = NULL;
    int ret, got_frame;
    int pixel_w = 0;
    int pixel_h = 0;
    int type = FRAME_TYPE_PREVIEW;
    mPacket = (AVPacket *)av_malloc(sizeof(AVPacket));
    preview_tid = SDL_CreateThread(preview_refresh_thread, NULL, NULL);
    if (preview_tid == NULL) {
        printf("avcode open failed!\n");
        return -1;
    }
    while (1) {
        SDL_WaitEvent(&event);
        if (event.type == REFRESH_EVENT || event.type == SDL_KEYDOWN) {
            if (av_read_frame(pFormatCtx, mPacket) >= 0) {
                if (mPacket->stream_index == videoStream) {
                    ret = avcodec_decode_video2(pCodecCtx, mFrame, &got_frame, mPacket);
                    if (ret < 0) {
                        printf("Decode Error.\n");
                        return -1;
                    }
                    if (got_frame) {
                        type = FRAME_TYPE_PREVIEW;
                        if (event.type == SDL_KEYDOWN) {
                            switch (event.key.keysym.sym) {
                                case SDLK_SPACE: {
                                    preview_pause = !preview_pause;
                                    break;
                                }
                                case SDLK_RETURN: { //capture
                                    type = FRAME_TYPE_SNAPSHOT;
                                    start_capture = 1;
                                    break;
                                }
                                default: {
                                    break;
                                }
                            }
                        }
                        sws_scale(img_convert_ctx, (const unsigned char* const*)mFrame->data, mFrame->linesize, 0, pCodecCtx->height, mFrameYUV->data, mFrameYUV->linesize);
                        mFrameYUV->width = mFrame->width;
                        mFrameYUV->height = mFrame->height;
                        void *data = malloc(mFrameYUV->width * mFrameYUV->height * sizeof(uint8_t) * 3);
                        process(mFrameYUV, data, type);
                        free(data);
                        SDL_UpdateTexture(mTexture, NULL, mFrameYUV->data[0], mFrameYUV->linesize[0]);
                        SDL_RenderClear(mRender);
                        SDL_RenderCopy(mRender, mTexture, NULL, NULL);
                        SDL_RenderPresent(mRender);
                    }
                }
                av_free_packet(mPacket);
            } else {
                thread_exit = 1;
                break;
            }
        }else if (event.type == SDL_WINDOWEVENT) {
			SDL_GetWindowSize(mWin, &pixel_w, &pixel_h);
        } else if (event.type == SDL_QUIT) {
            thread_exit = 1;
            break;
        } else if (event.type == BREAK_EVENT) {
            break;
        }
    }
    sws_freeContext(img_convert_ctx);
    SDL_Quit();
    av_free(out_buffer);
    av_free(mFrame);
    av_free(mFrameYUV);
    av_free(mPacket);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
    return 0;
}

int FrameGenerator::process(AVFrame* pFrame, void *data, int type)
{
    AVNV21ToNV21(data, pFrame);
    gFrame.frame = data;
    gFrame.w = pFrame->width;
    gFrame.h = pFrame->height;
    gFrame.stride = 64;
    gFrame.scanline = 64;
    gFrame.type = (pandora::FrameType)type;
    gFrame.format = FRAME_FORMAT_YUV_420_NV21;
    cv.notify_all(); //唤醒主线程处理图像数据
    while (!g_process_finished) ; //循环直到主线程处理完成
    NV21ToAVNV21(data, pFrame);
    g_process_finished = 0;

    return 0;
}

int FrameGenerator::AVyuv420Toyuv420(void *data, AVFrame* frame)
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

int FrameGenerator::AVNV21ToNV21(void *data, AVFrame* frame)
{
    int i, j;

    for (i = 0; i < frame->height; i++) {
        memcpy(data + frame->width * i,
            frame->data[0] + frame->linesize[0] * i,
            frame->width);
    }

    for (j = 0; j < frame->height / 2; j++) {
            memcpy(data + frame->width * i + frame->width * j,
                frame->data[1] + frame->linesize[1] * j,
                frame->width);
        }

    return 0;
}

int FrameGenerator::NV21ToAVNV21(void *data, AVFrame* frame)
{
    int i, j;

    for (i = 0; i < frame->height; i++) {
        memcpy(frame->data[0] + frame->linesize[0] * i,
            data + frame->width * i,
            frame->width);
    }

    for (j = 0; j < frame->height / 2; j++) {
        memcpy(frame->data[1] + frame->linesize[1] * j,
            data + frame->width * i + frame->width * j,
            frame->width);
    }
    return 0;
}


