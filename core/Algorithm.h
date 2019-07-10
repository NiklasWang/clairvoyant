#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include <stdarg.h>
#include "AlgorithmBase.h"
#include "GlobalTraits.h"

namespace pandora {

template <typename T, typename Trait = AlgTraits<T> >
class Algorithm :
    public AlgorithmBase<T, Trait>,
    virtual public noncopyable {

public:
    typedef T                           AlgorithmT;
    typedef typename Trait::TaskType    TaskTypeT;
    typedef typename Trait::ResultType  ResultTypeT;
    typedef typename Trait::UpdateType  UpdateTypeT;
    typedef typename Trait::ParmType    ParmTypeT;
    typedef typename Trait::RequestType RequestTypeT;
    typedef typename Trait::StatusType  StatusTypeT;
    typedef Algorithm<T, Trait>         MyType;

    using AlgorithmBase<T, Trait>::mResult;
    using AlgorithmBase<T, Trait>::mConstructed;
    using AlgorithmBase<T, Trait>::mAlgType;
    using AlgorithmBase<T, Trait>::mCaps;

public:
    static Algorithm *create(
        std::string algName = "noname_algorithm",
        std::string threadName = "noname_algorithm_thread",
        bool frameDrop = false, uint32_t maxQueueSize = 1,
        uint32_t argNum = 0, ...);

public:
    int32_t construct(uint32_t argNum, va_list va);
    virtual ~Algorithm();

private:
    int32_t initAlgorithm() override;
    int32_t deinitAlgorithm() override;
    int32_t processNewTask(TaskTypeT &task) override;
    int32_t processNewTask(TaskTypeT &in, TaskTypeT &out) override;
    int32_t processUpdate(UpdateTypeT &update) override;
    int32_t processParms(ParmTypeT &parm) override;
    int32_t processQueryStatus(StatusTypeT &status) override;
    AlgorithmT *getAlgorithmInDangerInternal() override;


private:
    Algorithm(std::string &algName, std::string &threadName,
        bool frameDrop, uint32_t maxQueueSize);

private:
    AlgorithmT *mAlg;
    ModuleType  mModule;
};

};

#include "Algorithm.hpp"

#endif
