#ifndef _PAL_TESTER_H_
#define _PAL_TESTER_H_

#include "Pal.h"
#include "PalParms.h"
#include "SemaphoreTimeout.h"

namespace pandora {

#define TEST_LOOP_TIMES 20

class PalTester
{
public:
    int32_t run();

public:
    PalTester(Pal *pal, uint32_t times = TEST_LOOP_TIMES);
    ~PalTester();

private:
    int32_t runPalTest();
    uint32_t getCaseCntInOneLoop();
    uint32_t getPalParmIndex(uint32_t cnt, GetParamType type);
    uint32_t getPalParmIndex(uint32_t cnt, SetParamType type);
    uint32_t getPalParmIndex(uint32_t cnt, ConfigType type);
    uint32_t getPalParmIndex(uint32_t cnt, OtherType type);
    int32_t runPalTestCase(uint32_t cnt);

private:
    struct TestCaseInfo {
        PalParmType type;
        int32_t     detail;
    };
    int32_t verifyTestResult(uint32_t lid,
        uint32_t rid, TestCaseInfo &result);

private:
    ModuleType mModule;
    Pal       *mPal;
    PalParms  *mPalParm;
    bool       mRuning;
    uint32_t   mRuningTimes;
    ThreadPoolEx    *mThreads;
    SemaphoreTimeout mSem;
    uint32_t         mFinishedCnt;
    RWLock           mWorkingLock;
    static const int32_t mDefaultMultiExp[];
    static const uint32_t     mDefaultDataMaxSize;
    static const char * const mDefaultDataToApp;
};

};

#endif
