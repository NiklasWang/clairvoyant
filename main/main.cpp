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
#include "CameraParameters.h"
#include "AlgorithmType.h"

using namespace std;
using namespace pandora;


#define SIZE (20*1024*1024)

int user_interface(CameraParameters *param)
{
    int rc = NO_ERROR;
    int algo = 0;
    int algoSwitch = 0;
re_start:
    cout << "select algo:" << endl;
    cout << ALG_BEAUTY_FACE <<".\tbeauty_face" <<endl;
    cout << ALG_WATER_MARK <<".\twater mark" << endl;
    cout << ALG_NIGHT_SHOT <<".\tnight shot" << endl;
    cout << ALG_MAX_INVALID << ".\tquit" << endl;
    cin >> algo;
    switch(algo)
    {
        case ALG_BEAUTY_FACE : {
            param->set(CameraParameters::KEY_BEAUTY_FACE, CameraParameters::ON);
            break;
        }
        case ALG_WATER_MARK : {
            param->set(CameraParameters::KEY_WATER_MARK, CameraParameters::ON);
            break;
        }
        case ALG_NIGHT_SHOT : {
            param->set(CameraParameters::KEY_NIGHT_SHOT, CameraParameters::ON);
            break;
        }
        case ALG_MAX_INVALID : {
            rc = -1;
            break;
        }
        default : {
            cout << "enter err" << endl;
            goto re_start;
        }
    }
    return rc;
}

int main(int argc, char *argv[])
{
    LOGE(MODULE_OTHERS, "start.....");

    int rc = NO_ERROR;

    PlatformOpsIntf *mPlatformOps = new pandora::PlatformOps();
    pandora::PandoraSingleton *mPandora;
    CameraParameters *param = new CameraParameters();
    while (1) {
        rc = user_interface(param);
        if (rc != NO_ERROR) {
            return 0;
        }

        if (mPlatformOps != NULL) {
            mPandora = pandora::PandoraSingleton::getInstance(mPlatformOps);
        }

        if (mPandora == NULL) {
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
        void *data = (void *)param;
        mPandora->onParameterAvailable(data);
        mPandora->takePicture();
        mPandora->onFrameReady(frame);
        mPandora->pictureTaken();
    }
    LOGE(MODULE_OTHERS, "success");
    return 0;
}
