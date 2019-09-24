#ifndef _FRAME_PROVIDER_H
#define _FRAME_PROVIDER_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "avcodec.h"
#include "avformat.h"
#include "swscale.h"
#include "avdevice.h"
#include "libavutil/imgutils.h"
#include "SDL.h"
#ifdef __cplusplus
};
#endif

enum {
    MAIN_THREAD_RUN = 1,
    PROVIDER_THREAD_RUN,
};

class FrameGenerator {
public:
    FrameGenerator();
    ~FrameGenerator();
    int Init();
    int start();
    static int preview_refresh_thread(void *opaque);
    static int process(AVFrame* pFrame, void *data, int type);
    static int AVyuv420Toyuv420(void *data, AVFrame* frame);
    static int AVNV21ToNV21(void *data, AVFrame* frame);
    static int NV21ToAVNV21(void *data, AVFrame* frame);
private:
    AVCodecContext  *pCodecCtx;
    AVFormatContext *pFormatCtx;
    AVFrame	        *mFrame;
    AVFrame         *mFrameYUV;
    AVPacket        *mPacket;
    SDL_Window      *mWin;
    SDL_Renderer    *mRender;
    SDL_Texture     *mTexture;
    SDL_Event       event;
    int             videoStream;
    unsigned char       *out_buffer;
    struct SwsContext   *img_convert_ctx;
};

#endif

