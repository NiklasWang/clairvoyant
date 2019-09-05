#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include "Pandora.h"
#include "PlatformOpsIntf.h"
#include "PandoraSingleton.h"
#include "PlatformOps.h"
#include "Common.h"
#include "Parameters.h"
#include "PlatformParameters.h"
#include "AlgorithmType.h"
#include "test.h"

using namespace pandora;


int main(int argc, char *argv[])
{
    LOGE(MODULE_OTHERS, "start.....");

    int rc = NO_ERROR;
    void *data;
    FrameInfo frame;

    PlatformOpsIntf *mPlatformOps = new pandora::PlatformOps();
    pandora::PandoraSingleton *mPandora;
    while (1) {
        if (mPlatformOps != NULL) {
            mPandora = pandora::PandoraSingleton::getInstance(mPlatformOps);
        }

        if (mPandora == NULL) {
            LOGE(MODULE_OTHERS, "fail to new");
            return -1;
        }

        rc = transfer(&data, frame);
        if (rc != 0) {
            return rc;
        }
        mPandora->onParameterAvailable(data);
        mPandora->takePicture();
        mPandora->onFrameReady(frame);
        mPandora->pictureTaken();
    }
    LOGE(MODULE_OTHERS, "success");

    return rc;
}
