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
#include "Logs.h"
#include "Common.h"

using namespace std;
using namespace pandora;


#define SIZE (20*1024*1024)

int main(int argc, char *argv[])
{
    PlatformOpsIntf *mPlatformOps = new pandora::PlatformOps();
    pandora::PandoraSingleton *mPandora;
    LOGE(MODULE_OTHERS, "start.....");
    if(mPlatformOps != NULL)
    {
        mPandora = pandora::PandoraSingleton::getInstance(mPlatformOps);
    }
    if(mPandora == NULL)
    {
        LOGE(MODULE_OTHERS, "fail to new");
        return -1;
    }

	char *fileName = "../1_water_mark_input_1566972572_dump_4096x3072.nv21";
    int fd = open(fileName, O_RDWR);
    if (fd < 0) {
        LOGE(MODULE_OTHERS,"fail open %s", fileName);
        return -1;
    }
    void *buf = malloc(SIZE);
    size_t size = read(fd, buf, SIZE);

    FrameInfo frame;
    frame.frame = buf;
    frame.w = 4096;
    frame.h = 3072;
    frame.stride = 64;
	frame.scanline = 64;
    frame.type = FRAME_TYPE_SNAPSHOT;
    frame.format = FRAME_FORMAT_YUV_420_NV21;
    void *data = NULL;
    mPandora->onParameterAvailable(data);
    mPandora->onFrameReady(frame);
    LOGE(MODULE_OTHERS, "success");


    return 0;
}
