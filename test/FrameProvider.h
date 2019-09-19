#ifndef _FRAME_PROVIDER_H
#define _FRAME_PROVIDER_H

#include <thread>
#include <condition_variable>

#ifdef __cplusplus
extern "C"
{
#endif
#include "avcodec.h"
#include "avformat.h"
#include "swscale.h"

#ifdef __cplusplus
};
#endif

enum {
    MAIN_THREAD_RUN = 1,
    PROVIDER_THREAD_RUN,
};

class FrameProvider {
public:
    FrameProvider();
    ~FrameProvider();
    int Init();
    static void startProviderThread();
    static int process(AVFrame* pFrame, void *data);
    static int AVFrame2yuv420(void *data, AVFrame* frame);
    static int AVFrame2NV21(void *data, AVFrame* frame);
};

#endif

