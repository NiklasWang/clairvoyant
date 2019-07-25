#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include "Pandora.h"

using namespace std;
using namespace pandora;


#define SIZE (20*1024*1024)

int main(int argc, char *argv[])
{
#if 1
    PlatformOpsIntf *platform = NULL;;
    PandoraInterface* pand = new Pandora(platform);

    if(pand == NULL)
    {
        cout<<"fail to new"<<endl;
        return -1;
    }
#endif
    int fd = open("./1.yuv", O_RDWR);
    if (fd < 0) {
        cout<<"fail open"<<endl;
        return -1;
    }
    void *buf = malloc(SIZE);
    size_t size = read(fd, buf, SIZE);
    FrameInfo frame;
    frame.frame = buf;
    frame.w = 3840;
    frame.h = 2160;
    frame.stride = frame.w;
    frame.type = FRAME_TYPE_SNAPSHOT;
    pand->onFrameReady(frame);
    cout<<"success"<<endl;


    return 0;
}
