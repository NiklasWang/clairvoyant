#include "BufItem.h"

namespace pandora {

template <>
struct BufItem<Global> :
    public RefBase {
public:
    void *getBuf() const
    {
        return mBuf;
    }

    typename AlgTraits<Global>::TaskType getTask() const
    {
        return mTask;
    }


    size_t getSize() const
    {
        return mSize;
    }

    size_t replace(typename AlgTraits<Global>::TaskType &task, size_t allocsize)
    {
        ASSERT_LOG(mModule, task.size <= allocsize,
            "Invalid data size %d and alloc size %d", task.size, allocsize);

        if (allocsize != mSize) {
            if (NOTNULL(mBuf)) {
                SECURE_FREE(mBuf);
            }
            mBuf = Malloc(allocsize);
            if (NOTNULL(mBuf)) {
                mSize = allocsize;
                memcpy(mBuf, task.data, task.size);
                mTask = task;
                mTask.data = mBuf;
            } else {
                LOGE(mModule, "Failed to create buf item %d bytes", allocsize);
            }
        } else {
            memcpy(mBuf, task.data, mSize);
            mTask = task;
            mTask.data = mBuf;
        }

        return mSize;
    }

public:

    BufItem(typename AlgTraits<Global>::TaskType &task, size_t allocsize) :
        mBuf(NULL),
        mSize(0),
        mModule(MODULE_BUFFER_MANAGER),
        mTask(task)
    {
        if (NOTNULL(mTask.data) && allocsize > 0) {
            replace(task, allocsize);
        } else {
            LOGE(mModule, "Failed to create buf item, buf %p size %d",
                mTask.data, allocsize);
        }
    }

    ~BufItem()
    {
        if (NOTNULL(mBuf)) {
            SECURE_FREE(mBuf);
        }
    }

    BufItem<Global> &operator=(const BufItem<Global> &rhs)
    {
        if (this != &rhs) {
            if (NOTNULL(mBuf)) {
                SECURE_FREE(mBuf);
            }

            void *buf = rhs.getBuf();
            size_t size = rhs.getSize();
            if (NOTNULL(buf) && size > 0) {
                mBuf = Malloc(size);
                if (NOTNULL(mBuf)) {
                    mSize = size;
                    memcpy(mBuf, buf, size);
                    mTask = rhs.mTask;
                    mTask.data = mBuf;
                } else {
                    LOGE(mModule, "Failed to create buf item %d bytes", size);
                }
            } else {
                LOGE(mModule, "Failed to create buf item, buf %p size %d",
                    buf, size);
            }
        }

        return *this;
    }

    bool operator==(const BufItem<Global>&rhs) const
    {
        bool rc = false;
        if ((rhs.getSize() == mSize) &&
            (!memcmp(rhs.getBuf(), mBuf, rhs.getSize()))) {
            rc = true;
        }

        return rc;
    }

    bool operator!=(const BufItem<Global>&rhs) const
    {
        return !(*this == rhs);
    }

private:
    void    *mBuf;
    size_t   mSize;
    ModuleType mModule;
    typename AlgTraits<Global>::TaskType mTask;
};

};
