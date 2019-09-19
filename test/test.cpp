#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

#include "test.h"
#include "AlgorithmType.h"
#include "PlatformParameters.h"

using namespace std;
using namespace pandora;


#define SIZE (20*1024*1024)

Test::Test()
{}
Test::~Test()
{}

int Test::user_interface(Parameters *param)
{
    int rc = 0;
    int algo = 0;
    int algoSwitch = 0;
re_start:
    cout << "select algo:" << endl;
    cout << ALG_BEAUTY_FACE <<".\tbeauty face" <<endl;
    cout << ALG_WATER_MARK <<".\twater mark" << endl;
    cout << ALG_NIGHT_SHOT <<".\tnight shot" << endl;
    cout << ALG_MAX_INVALID << ".\tquit" << endl;
    cin >> algo;
    switch (algo) {
        case ALG_BEAUTY_FACE : {
            param->set(KEY_BEAUTY_FACE, ON);
            break;
        }
        case ALG_WATER_MARK : {
            param->set(KEY_WATER_MARK, ON);
            break;
        }
        case ALG_NIGHT_SHOT : {
            param->set(KEY_NIGHT_SHOT, ON);
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

int Test::transfer(void **data, FrameInfo &frame)
{
    int rc = 0;
    Parameters *param = new Parameters();
    rc = user_interface(param);
    if (rc != 0) {
        return -1;
    }

    char *fileName = "../test/1_water_mark_input_1566972572_dump_4096x3072.nv21";
    int fd = open(fileName, O_RDWR);
    if (fd < 0) {
        perror("fail open\n");
        return -1;
    }
    void *buf = malloc(SIZE);
    size_t size = read(fd, buf, SIZE);

    FrameInfo m_frame;
    m_frame.frame = buf;
    m_frame.w = 4096;
    m_frame.h = 3072;
    m_frame.stride = 64;
    m_frame.scanline = 64;
    m_frame.type = FRAME_TYPE_SNAPSHOT;
    m_frame.format = FRAME_FORMAT_YUV_420_NV21;
    *data = param;
    frame = m_frame;

    return 0;
}



