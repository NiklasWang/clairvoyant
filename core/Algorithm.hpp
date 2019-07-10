#ifndef _ALGORITHM_HPP_
#define _ALGORITHM_HPP_

#include "Algorithm.h"

namespace pandora {

template <typename T, typename Trait>
Algorithm<T, Trait>::Algorithm(
    std::string &algoName, std::string &threadName,
    bool frameDrop, uint32_t maxQueueSize) :
    AlgorithmBase<T, Trait>(algoName, threadName, frameDrop, maxQueueSize),
    mAlg(NULL),
    mModule(MODULE_ALGORITHM_TEMPLATE)
{
    mResult.valid = false;
}

template <typename T, typename Trait>
Algorithm<T, Trait>::~Algorithm()
{
    if (mConstructed) {
        AlgorithmBase<T, Trait>::destruct();
    }
    if (mAlg) {
        delete mAlg;
        mAlg = NULL;
    }
}

template <typename T, typename Trait>
Algorithm<T, Trait> *
    Algorithm<T, Trait>::create(
    std::string algName, std::string threadName,
    bool frameDrop, uint32_t maxQueueSize, uint32_t argNum, ...)
{
    int32_t rc = NO_ERROR;
    Algorithm<T, Trait> *algorithm = NULL;

    algorithm = new Algorithm<T, Trait>(
        algName, threadName, frameDrop, maxQueueSize);
    if (ISNULL(algorithm)) {
        LOGE(MODULE_ALGORITHM_TEMPLATE, "Failed to create Algorithm");
        rc = NO_MEMORY;
    }

    if (SUCCEED(rc)) {
        va_list va;
        va_start(va, argNum);
        rc = algorithm->construct(argNum, va);
        va_end(va);
        if (!SUCCEED(rc)) {
            LOGE(MODULE_ALGORITHM_TEMPLATE,
                "Failed to construct algorithm, %d", rc);
            delete algorithm;
        }
    }

    return SUCCEED(rc) ? algorithm : NULL;
}

template <typename T, typename Trait>
int32_t Algorithm<T, Trait>::construct(
    uint32_t argNum, va_list va)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        mAlg = new AlgorithmT(argNum, va);
        if (ISNULL(mAlg)) {
            LOGE(mModule, "Failed to create TAlgorithm");
            rc = NO_MEMORY;
        } else {
            typename Trait::getTypeF fnGetType = Trait::fnGetType;
            mAlgType = (mAlg->*fnGetType)();
            mResult.type = mAlgType;
        }
    }

    if (SUCCEED(rc)) {
        rc = AlgorithmBase<T, Trait>::construct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct algorithm base, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        typename Trait::queryCapsF fnQueryCaps = Trait::fnQueryCaps;
        rc = (mAlg->*fnQueryCaps)(mCaps);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to query algorithm capabilities, %d", rc);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t Algorithm<T, Trait>::initAlgorithm()
{
    int32_t rc = NOT_INITED;

    if (!ISNULL(mAlg)) {
        typename Trait::initF fnInit = Trait::fnInit;
        rc = (mAlg->*fnInit)();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to init algorithm, %d", rc);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t Algorithm<T, Trait>::deinitAlgorithm()
{
    int32_t rc = NOT_INITED;

    if (!ISNULL(mAlg)) {
        typename Trait::deinitF fnDeinit = Trait::fnDeinit;
        rc = (mAlg->*fnDeinit)();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to deinit algorithm, %d", rc);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t Algorithm<T, Trait>::processNewTask(TaskTypeT &task)
{
    int32_t rc = NOT_INITED;

    if (!ISNULL(mAlg)) {
        typename Trait::processF fnProcess = Trait::fnProcess;
        rc = (mAlg->*fnProcess)(task, mResult);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to process, %d", rc);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t Algorithm<T, Trait>::processNewTask(TaskTypeT &in, TaskTypeT &out)
{
    int32_t rc = NOT_INITED;

    if (!ISNULL(mAlg)) {
        typename Trait::processExF fnProcessEx = Trait::fnProcessEx;
        rc = (mAlg->*fnProcessEx)(in, out, mResult);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to ProcessEx, %d", rc);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t Algorithm<T, Trait>::processParms(ParmTypeT &parm)
{
    int32_t rc = NOT_INITED;

    if (!ISNULL(mAlg)) {
        typename Trait::setParmF fnSetParm = Trait::fnSetParm;
        rc = (mAlg->*fnSetParm)(parm);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to set parm, %d", rc);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t Algorithm<T, Trait>::processUpdate(UpdateTypeT &update)
{
    int32_t rc = NOT_INITED;

    if (!ISNULL(mAlg)) {
        typename Trait::updateF fnUpdate = Trait::fnUpdate;
        rc = (mAlg->*fnUpdate)(update);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to update, %d", rc);
        }
    }

    return rc;
}

template <typename T, typename Trait>
int32_t Algorithm<T, Trait>::processQueryStatus(StatusTypeT &status)
{
    int32_t rc = NOT_INITED;

    if (!ISNULL(mAlg)) {
        typename Trait::queryStatusF fnQueryStatus = Trait::fnQueryStatus;
        rc = (mAlg->*fnQueryStatus)(status);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to query status, %d", rc);
        }
    }

    return rc;
}

template <typename T, typename Trait>
typename Algorithm<T, Trait>::AlgorithmT *
    Algorithm<T, Trait>::getAlgorithmInDangerInternal()
{
    return mAlg;
}

};

#endif
