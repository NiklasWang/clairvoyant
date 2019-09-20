#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <condition_variable>


#include "Pandora.h"
#include "PlatformOpsIntf.h"
#include "PandoraSingleton.h"
#include "PlatformOps.h"
#include "Common.h"
#include "Parameters.h"
#include "PlatformParameters.h"
#include "AlgorithmType.h"
#include "test.h"
#include "FrameProvider.h"
#include "AlgorithmHelper.h"

using namespace pandora;
using namespace std;

extern int thread_exit;
extern int preview_pause;
extern int start_capture;
extern int g_process_finished;
std::mutex mtx;
std::condition_variable cv;


FrameInfo gFrame; //use to thread communication

void startMainThread(void *arg, void *param)
{
    cout << "MainThread run..." << endl << endl;
    PandoraSingleton *pandora = (PandoraSingleton *)arg;
    pandora->onParameterAvailable(param);
    pandora->startPreview();
    thread_exit = 0;
    preview_pause = 0;
    unique_lock<std::mutex> lock(mtx);

	while (!thread_exit) {
        cv.wait(lock);
        if (start_capture) {
            pandora->takePicture();
            start_capture = 0;
        }
        pandora->onFrameReady(gFrame);
        if (gFrame.type == FRAME_TYPE_SNAPSHOT) {
            dumpNV21ToJpeg(gFrame.frame, "output", gFrame.w, gFrame.h, gFrame.stride, gFrame.scanline);
        }
        g_process_finished = 1;
	}
    cout << "MainThread exit..." << endl << endl;
}

int main(int argc, char *argv[])
{
    int rc = NO_ERROR;
    PlatformOpsIntf *mPlatformOps = new pandora::PlatformOps();
    pandora::PandoraSingleton *mPandora;
    if (mPlatformOps != NULL) {
        mPandora = pandora::PandoraSingleton::getInstance(mPlatformOps);
    }

    if (mPandora == NULL) {
        LOGE(MODULE_OTHERS, "fail to new");
        return -1;
    }
    Parameters *param = new Parameters();
    Test *test = new Test();
    rc = test->user_interface(param);
    if (rc != 0) {
        return -1;
    }
    FrameProvider *provider = new FrameProvider();
    std::thread  t1(startMainThread, mPandora, param);

    provider->Init();
    provider->start();
    t1.join();

    return rc;
}

