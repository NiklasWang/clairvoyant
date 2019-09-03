#include "PalImpl.h"
#include "Logs.h"
#include "ConfigInterface.h"
#include "PlatformOpsIntf.h"

namespace pandora {

PalImpl::PalImpl(PlatformOpsIntf *platform) :
    mConstructed(false),
    mModule(MODULE_PAL_IMPL),
    mPlatform(platform),
    mThreads(NULL)
{
}

PalImpl::~PalImpl()
{
    if (mConstructed) {
        destruct();
    }
}

int32_t PalImpl::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mPlatform)) {
            LOGF(mModule, "Fatal: Platform hal not given");
            rc = NOT_INITED;
        } else {
            mPlatform->whoamI();
        }
    }

    if (SUCCEED(rc)) {
        mThreads = ThreadPoolEx::getInstance();
        if (ISNULL(mThreads)) {
            LOGE(mModule, "Failed to create thread scheduler");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mParmsBuf)) {
            mParmsBuf = new ObjectBufferEx<PalParms>();
            if (ISNULL(mParmsBuf)) {
                LOGE(mModule, "Failed to create parameter buffer");
                rc = UNKNOWN_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mTaskBuf)) {
            mTaskBuf = new ObjectBufferEx<TaskInf>();
            if (ISNULL(mTaskBuf)) {
                LOGE(mModule, "Failed to create task info buffer");
                rc = UNKNOWN_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
        LOGD(mModule, "PAL impl constructed");
    }

    return RETURNIGNORE(rc, ALREADY_EXISTS);
}

int32_t PalImpl::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        mThreads->removeInstance();
    }

    if (!SUCCEED(rc)) {
        mConstructed = true;
        LOGE(mModule, "Failed to destruct pal impl");
    } else {
        LOGD(mModule, "Pal impl destructed");
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

ParmCategoryType PalImpl::getCategory(PalParms *parm)
{
    int32_t rc = NO_ERROR;
    sp<ConfigInterface> configs = NULL;
    ParmsCategory category;
    ParmCategoryType result = PARM_CATEGORY_MAX_INVALID;
    ASSERT_LOG(mModule, NOTNULL(parm), "parm shouldn't be NULL");

    if (SUCCEED(rc)) {
        configs = mPlatform->getConfig();
        if (ISNULL(configs)) {
            LOGE(mModule, "Failed to get configs");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        rc = configs->getParmCategory(category);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to get parameter category");
        }
    }

    if (SUCCEED(rc)) {
        for (uint32_t i = 0; i < category.size; i++) {
            if (parm->type == category.data[i].type) {
                if (parm->type == PARM_TYPE_GET &&
                    parm->g.type == category.data[i].u.g) {
                    result = category.data[i].category;
                    break;
                } else if (parm->type == PARM_TYPE_SET &&
                    parm->s.type == category.data[i].u.s) {
                    result = category.data[i].category;
                    break;
                } else if (parm->type == PARM_TYPE_CONFIG &&
                    parm->c.type == category.data[i].u.c) {
                    result = category.data[i].category;
                    break;
                } else if (parm->type == PARM_TYPE_OTHERS &&
                    parm->o.type == category.data[i].u.o) {
                    result = category.data[i].category;
                    break;
                }
            }
        }
    }

    return result;
}

int32_t PalImpl::waitForThread(ParmCategoryType category)
{
    int32_t rc = NO_ERROR;
    sp<ThreadInf> inf = NULL;

    if (SUCCEED(rc)) {
        RWLock::AutoWLock l(mThreadInfLock);
        for (auto &info : mThreadInf) {
            if (info->category == category) {
                inf = info;
                break;
            }
        }

        if (ISNULL(inf)) {
            inf = new ThreadInf();
            if (NOTNULL(inf)) {
                inf->category = category;
                mThreadInf.push_back(inf);
            } else {
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        inf->sem.wait();
    }

    return rc;
}

int32_t PalImpl::releaseThread(ParmCategoryType category)
{
    int32_t rc = NO_ERROR;
    sp<ThreadInf> inf = NULL;

    if (SUCCEED(rc)) {
        RWLock::AutoRLock l(mThreadInfLock);
        for (auto &info : mThreadInf) {
            if (info->category == category) {
                inf = info;
                break;
            }
        }
    }

    if (ISNULL(inf)) {
        rc = NOT_INITED;
        LOGE(mModule, "Thread info not found");
    }

    if (SUCCEED(rc)) {
        inf->sem.signal();
    }

    return rc;
}

int32_t PalImpl::processTask(TaskInf *task)
{
    assert(!ISNULL(task));

    uint32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        ParmCategoryType category = getCategory(task->getpp());
        if (category != PARM_CATEGORY_TYPE_FAST) {
            rc = waitForThread(category);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to wait thread for category %d",
                    category);
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = task->rc = doAction(task->getpp());
        if (!SUCCEED(rc)) {
            LOGD(mModule, "Failed to do %s", task->getpp()->whoamI());
        }
    }

    return rc;
}

int32_t PalImpl::taskDone(TaskInf *task, int32_t processrc)
{
    int32_t rc = NO_ERROR;

    if (!SUCCEED(processrc)) {
        LOGD(mModule, "Failed to process evt %s rc %d, trying to ignore",
            task->getpp()->whoamI(), processrc);
    }

    if (SUCCEED(rc)) {
        ParmCategoryType category = getCategory(task->getpp());
        if (category != PARM_CATEGORY_TYPE_FAST) {
            rc = releaseThread(category);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to release thread for category %d",
                    category);
            }
        }
    }

    return rc;
}

int32_t PalImpl::doAction(PalParms *parm)
{
    return mPlatform->requestHandler(parm);
}

int32_t PalImpl::onMetadataAvailable(const void *metadata)
{
    return mPlatform->onMetadataAvailable(metadata);
}

int32_t PalImpl::onParameterAvailable(const void *parm)
{
    return mPlatform->onParameterAvailable(parm);
}

int32_t PalImpl::onCommandAvailable(int32_t cmd, int32_t arg1, int32_t arg2)
{
    return mPlatform->onCommandAvailable(cmd, arg1, arg2);
}


PalImpl::TaskInf::TaskInf(ModuleType module) :
    Identifier(module),
    bufParm(false),
    bufMe(false),
    rc(NO_ERROR),
    parm(NULL),
    index(cnt++)
{
}

PalImpl::TaskInf::TaskInf(const PalImpl::TaskInf &rhs)
{
    if (this != &rhs) {
        index = cnt++;
    }
}

uint32_t PalImpl::TaskInf::id()
{
    return index;
}

PalParms *PalImpl::TaskInf::getpp()
{
    return parm;
}

void PalImpl::TaskInf::set(PalParms *p, bool buffered)
{
    parm = p;
    bufParm = buffered;
}

void PalImpl::TaskInf::clear()
{
    parm = NULL;
    bufParm = false;
    bufMe = false;
    rc = NO_ERROR;
}

uint32_t PalImpl::TaskInf::cnt = 0;

PalImpl::ThreadInf::ThreadInf() :
    sem(1),
    category(PARM_CATEGORY_MAX_INVALID)
{
}

};

