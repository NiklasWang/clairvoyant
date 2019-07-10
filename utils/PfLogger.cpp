#include "PfLogger.h"
#include "PfLoggerImpl.h"
#include "Common.h"

namespace pandora {

PfLogger *PfLogger::gInstance = NULL;

PfLogger *PfLogger::getInstance()
{
    return ISNULL(gInstance)  ? gInstance = new PfLogger() : gInstance;
}

int32_t PfLogger::start(const char *name, uint32_t times)
{
    return NOTNULL(mP) ? mP->start(name, times) : NOT_INITED;
}

int32_t PfLogger::stop(const char *name)
{
    return NOTNULL(mP) ? mP->stop(name) : NOT_INITED;
}

int32_t PfLogger::output()
{
    return NOTNULL(mP) ? mP->output() : NOT_INITED;
}

int32_t PfLogger::output(const char *name)
{
    return NOTNULL(mP) ? mP->output(name) : NOT_INITED;
}

void PfLogger::clear()
{
    if (NOTNULL(mP)) {
        mP->clear();
    }
}

int32_t PfLogger::clear(const char *name)
{
    return NOTNULL(mP) ? mP->clear(name) : NOT_INITED;
}


PfLogger::PfLogger() :
    mP(new PfLoggerImpl())
{
}

PfLogger::~PfLogger()
{
    if (NOTNULL(mP)) {
        mP->clear();
        delete mP;
        mP = NULL;
    }
    gInstance = NULL;
}

};

