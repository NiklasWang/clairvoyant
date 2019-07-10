#include "AlgorithmIntf.h"

namespace pandora {

uint32_t AlgTraits<Global>::TaskType::taskcnt = 0;

void IAlgResult::dump()
{
    LOGI(MODULE_ALGORITHM_HELPER, "Algorithm result: %s, task id %d",
        valid ? "valid" : "invalid", taskid);
}

int32_t IAlgorithm::processSync(TaskType &task, uint32_t taskId)
{
    return process(
        task, taskId, SYNC_TYPE,
        [](TaskType &) -> int32_t { return 0; });
}

int32_t IAlgorithm::processAsync(TaskType &task, uint32_t taskId,
    std::function<int32_t (TaskType &)> cb)
{
    return process(task, taskId, ASYNC_TYPE, cb);
}

int32_t IAlgorithm::processSync(TaskType &task, TaskType &result, uint32_t taskId)
{
    return process(
        task, result, taskId, SYNC_TYPE,
        [](TaskType &) -> int32_t { return 0; });
}

int32_t IAlgorithm::processAsync(TaskType &task, TaskType &result,
    uint32_t taskId, std::function<int32_t (TaskType &)> cb)
{
    return process(task, result, taskId, ASYNC_TYPE, cb);
}

int32_t IAlgorithm::configSync(ParmType &parm)
{
    return config(parm, SYNC_TYPE);
}

int32_t IAlgorithm::configAsync(ParmType &parm)
{
    return config(parm, ASYNC_TYPE);
}

int32_t IAlgorithm::updateSync(UpdateType &arg)
{
    return update(arg, SYNC_TYPE);
}

int32_t IAlgorithm::updateAsync(UpdateType &arg)
{
    return update(arg, ASYNC_TYPE);
}

const char *IAlgorithm::whoamI() const
{
    return getName();
}

AlgType IAlgorithm::myType() const
{
    return getType();
}

bool IAlgorithm::idle() const
{
    return !busy();
}

};
