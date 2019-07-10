#ifndef _ALGORITHM_BUF_HOLDER_
#define _ALGORITHM_BUF_HOLDER_

#include <vector>

#include "Common.h"
#include "GlobalTraits.h"
#include "BufItem.h"

namespace pandora {

template <typename T>
class BufHolder {
public:
    int32_t add(typename AlgTraits<T>::TaskType &task, size_t allocsize = 0);
    int32_t add(BufItem<T> &item);
    int32_t remove(uint32_t index);
    int32_t remove(BufItem<T> &item);
    int32_t replace(uint32_t index,
        typename AlgTraits<T>::TaskType &task, size_t allocsize = 0);
    int32_t replace(uint32_t index, BufItem<T> &item);
    void clear();
    void *operator[](uint32_t index);
    uint32_t size();
    void *getLastData();
    int32_t getLastTask(typename AlgTraits<T>::TaskType &frame);

public:
    BufHolder();
    ~BufHolder();
    BufHolder(const BufHolder &rhs);
    BufHolder &operator=(const BufHolder &rhs);
    bool operator==(const BufHolder &rhs);
    bool operator!=(const BufHolder &rhs);

private:
    uint32_t   mSize;
    ModuleType mModule;
    std::vector<sp<BufItem<T> > > mBufs;
};

};

#include "BufHolder.hpp"

#endif
