#ifndef _PANDORA_INTERFACE_INTERNAL_H_
#define _PANDORA_INTERFACE_INTERNAL_H_

#include "PandoraInterface.h"

namespace pandora {

bool checkValid(FrameFormat format);

FrameFormat getValidType(FrameFormat format);

const char *frameFormatName(FrameFormat format);

bool checkValid(FrameType type);

FrameType getValidType(FrameType type);

const char *frameTypeName(FrameType type);

};

#endif
