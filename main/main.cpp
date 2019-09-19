#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
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

using namespace pandora;
using namespace std;

std::mutex mtx;
std::condition_variable cv;
int ready = 0; //control thread

FrameInfo frame; //use to thread communication

void startMainThread(void *arg, void *param)
{
    cout << "start thread ******main******" << endl << endl;
    PandoraSingleton *pandora = (PandoraSingleton *)arg;
    pandora->onParameterAvailable(param);

    unique_lock<std::mutex> lock(mtx);
    while (1) {
        while (ready != MAIN_THREAD_RUN) {
            cv.wait(lock);
        }
        pandora->takePicture();
        pandora->onFrameReady(frame);
        ready = PROVIDER_THREAD_RUN;
        cv.notify_all();
    }
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
    provider->Init();
    ready = PROVIDER_THREAD_RUN;

    std::thread  t1(startMainThread, mPandora, param);
    std::thread  t2(&provider->startProviderThread);
    t1.join();
    t2.join();

    return rc;
}

