#ifndef ALGORITHM_PROPERTIES_H_
#define ALGORITHM_PROPERTIES_H_

#include "Common.h"
#include "AlgorithmType.h"
#include "RefBase.h"

namespace pandora {

struct AlgProperties :
    public RefBase {
public:
    const char *getName(AlgType type) {
        return prop[getValidType(type)].name;
    }
    const char *getThreadName(AlgType type) {
        return prop[getValidType(type)].threadName;
    }
    bool isDropFrame(AlgType type) {
        return prop[getValidType(type)].frameDrop;
    }
    uint32_t getMaxQueueSize(AlgType type) {
        return prop[getValidType(type)].maxQueueSize;
    }
    uint32_t getArgsNum(AlgType type) {
        return prop[getValidType(type)].argsNum;
    }

private:
    AlgType getValidType(AlgType type) {
        return (type < 0 || type > ALG_MAX_INVALID) ?
            ALG_MAX_INVALID : type;
    }

public:
    struct AlgorithmProperty {
        const char *name;
        const char *threadName;
        bool frameDrop;
        uint32_t maxQueueSize;
        uint32_t argsNum;
    };

private:
    static const AlgorithmProperty prop[];
};

};

#endif
