#ifndef _TEST_H
#define _TEST_H

#include "Parameters.h"
#include "Pandora.h"

using namespace pandora;


class Test {
public:
    Test();
    ~Test();
    int Init();
    int transfer(void **data, FrameInfo &frame);
    int user_interface(Parameters *param);
};


#endif

