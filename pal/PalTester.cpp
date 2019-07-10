#include "Common.h"
#include "Pal.h"
#include "PalParms.h"
#include "MemMgmt.h"
#include "PfLogger.h"
#include "PandoraCore.h"
#include "ThreadPoolEx.h"
#include "PalTester.h"

namespace pandora {

#define ALL_CASES_MAX_FINISH_TIME 1000 // ms

const int32_t  PalTester::mDefaultMultiExp[]    = {-6, 0, 6};
const uint32_t PalTester::mDefaultDataMaxSize   = 32;
const char * const PalTester::mDefaultDataToApp = "Pal Test Program.";

int32_t PalTester::run()
{
    int32_t rc = NO_ERROR;

    RWLock::AutoRLock l(mWorkingLock);
    PfLogger::getInstance()->start("Pal_test_cases", 1);

    rc = runPalTest();
    if (!SUCCEED(rc)) {
        LOGE(mModule, "Test Failed! During running "
            "Pal test cases, check logs, %d", rc);
    }

    PfLogger::getInstance()->stop("Pal_test_cases");

    return rc;
}

PalTester::PalTester(Pal *pal, uint32_t times) :
    mModule(MODULE_UTILS),
    mPal(pal),
    mPalParm(NULL),
    mRuning(false),
    mRuningTimes(times),
    mThreads(NULL),
    mSem(ALL_CASES_MAX_FINISH_TIME),
    mFinishedCnt(0)
{
    ASSERT_LOG(mModule, NOTNULL(mPal), "Pal can't be NULL, Suicide.");
}

PalTester::~PalTester()
{
    RWLock::AutoWLock l(mWorkingLock);

    mRuning = true;

    if (NOTNULL(mThreads)) {
        mThreads->removeInstance();
    }

    if (NOTNULL(mPalParm)) {
        SECURE_FREE(mPalParm);
    }
}

int32_t PalTester::runPalTest()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (mRuning) {
            LOGI(mModule, "Test Runing, skip this time.");
            rc = NOT_NEEDED;
        } else {
            mRuning = true;
        }
    }

    if (SUCCEED(rc)) {
        size_t len = getCaseCntInOneLoop() * mRuningTimes;
        if (ISNULL(mPalParm)) {
            len *= sizeof(PalParms);
            mPalParm = (PalParms *)Malloc(len);
            if (ISNULL(mPalParm)) {
                LOGE(mModule, "Failed to alloc %d bytes.", len);
            } else {
                LOGI(mModule, "Alloc %d (%d*%d*%d) bytes for Pal"
                    " test.", len, mRuningTimes,
                    getCaseCntInOneLoop(), sizeof(PalParms));
            }
        }
        if (NOTNULL(mPalParm)) {
            memset(mPalParm, 0, len);
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mThreads)) {
            mThreads = ThreadPoolEx::getInstance();
            if (ISNULL(mThreads)) {
                LOGE(mModule, "Failed to get thread pool");
                rc = UNKNOWN_ERROR;
            }
        }
    }

    if (SUCCEED(rc)) {
        for (uint32_t i = 0; i < mRuningTimes; i++) {
            rc = mThreads->run(
                [this, i]() -> int32_t {
                    int32_t _rc = NO_ERROR;
                    LOGD(mModule, "Start runing test case %d/%d", i, mRuningTimes);
                    _rc = runPalTestCase(i);
                    mFinishedCnt++;
                    LOGD(mModule, "Finished runing test case %d/%d, "
                        "remaining %d cases.", i, mRuningTimes,
                        mRuningTimes - mFinishedCnt);
                    mSem.signal();
                    return _rc;
                }
            );
        }
    }

    if (SUCCEED(rc)) {
        for (uint32_t i = 0; i < mRuningTimes; i++) {
            rc = mSem.wait();
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Test FAILED! %d/%d cases remaining but "
                    "timedout in %d ms.", mRuningTimes - mFinishedCnt,
                    mRuningTimes, ALL_CASES_MAX_FINISH_TIME);
            } else {
                LOGI(mModule, "One test case finished, %d/%d remaining.",
                    mRuningTimes - mFinishedCnt, mRuningTimes);
            }
        }
    }

    if (SUCCEED(rc)) {
        TestCaseInfo result;
        for (uint32_t i = 0; i < mRuningTimes - 1; i++) {
            rc = verifyTestResult(i, i + 1, result);
            if (!SUCCEED(rc)) {
                const char *type = mPalParm->getName(result.type);
                const char *curr = NULL;
                switch (result.type) {
                    case PARM_TYPE_GET:
                        curr = mPalParm->getName(
                            static_cast<GetParamType>(result.detail));
                        break;
                    case PARM_TYPE_SET:
                        curr = mPalParm->getName(
                            static_cast<SetParamType>(result.detail));
                        break;
                    case PARM_TYPE_CONFIG:
                        curr = mPalParm->getName(
                            static_cast<ConfigType>(result.detail));
                        break;
                    case PARM_TYPE_OTHERS:
                        curr = mPalParm->getName(
                            static_cast<OtherType>(result.detail));
                        break;
                    case PARM_TYPE_MAX_INVALID:
                    default:
                        curr = "Unknown";
                        break;
                }
                LOGE(mModule, "Test FAILED! Expected all test result in "
                    "same cases are same, Actually the test result for "
                    "[%s] group [%s] case is different in round %d and "
                    "round %d.", type, curr, i, i + 1);
            }
        }
    }

    if (SUCCEED(rc)) {
        LOGI(mModule, "Test Succeed! All pass in Pal test, %d round.",
            mRuningTimes);
    }

    if (!SUCCEED(rc) || SUCCEED(rc)) {
        mRuning = false;
    }

    return RETURNIGNORE(rc, NOT_NEEDED);
}

uint32_t PalTester::getCaseCntInOneLoop()
{
    return GET_PARM_MAX_INVALID +
        SET_PARM_MAX_INVALID +
        CONFIG_MAX_INVALID + OTHER_TYPE_MAX_INVALID;
}

uint32_t PalTester::getPalParmIndex(uint32_t cnt, GetParamType type)
{
    return cnt * getCaseCntInOneLoop() + type;
}

uint32_t PalTester::getPalParmIndex(uint32_t cnt, SetParamType type)
{
    return getPalParmIndex(cnt, GET_PARM_MAX_INVALID) + type;
}

uint32_t PalTester::getPalParmIndex(uint32_t cnt, ConfigType type)
{
    return getPalParmIndex(cnt, SET_PARM_MAX_INVALID) + type;
}

uint32_t PalTester::getPalParmIndex(uint32_t cnt, OtherType type)
{
    return getPalParmIndex(cnt, CONFIG_MAX_INVALID) + type;
}

#define ADD_TEST_CASE_SYNC(TYPE, FUNC, ELE, DEBUG) \
    if (SUCCEED(rc)) { \
        parm = mPalParm + getPalParmIndex(cnt, TYPE); \
        rc = mPal->FUNC(parm->ELE, SYNC_TYPE); \
        if (!SUCCEED(rc)) { \
            if (DEBUG) { \
                const char *name = mPalParm->getName(TYPE); \
                LOGE(mModule, "Test case FAILED! When running " \
                    "case %s in round %d, SYNC mode, %d", \
                    name, cnt, rc); \
            } \
            RESETRESULT(rc); \
        } \
    }

#define ADD_TEST_CASE_ASYNC(TYPE, FUNC, ELE, DEBUG) \
    if (SUCCEED(rc)) { \
        parm = mPalParm + getPalParmIndex(cnt, TYPE); \
        rc = mPal->FUNC(parm->ELE, ASYNC_TYPE); \
        if (!SUCCEED(rc)) { \
            if (DEBUG) { \
                const char *name = mPalParm->getName(TYPE); \
                LOGE(mModule, "Test case FAILED! When running " \
                    "case %s in round %d, ASYNC mode, %d", \
                    name, cnt, rc); \
            } \
            RESETRESULT(rc); \
        } \
    }

#define ADD_TEST_CASE(TYPE, FUNC, ELE, DEBUG) \
    ADD_TEST_CASE_SYNC( TYPE, FUNC, ELE, DEBUG) \
    ADD_TEST_CASE_ASYNC(TYPE, FUNC, ELE, DEBUG)


void release_func(void *arg) {
    if (NOTNULL(arg)) {
        SECURE_FREE(arg);
    }
}

int32_t PalTester::runPalTestCase(uint32_t cnt)
{
    int32_t rc = NO_ERROR;
    PalParms *parm = NULL;

    ADD_TEST_CASE(GET_PARM_PREVIEW_SIZE,   getParm,   g.u.previewSize,    false);
    ADD_TEST_CASE(GET_PARM_PICTURE_SIZE,   getParm,   g.u.pictureSize,    false);
    ADD_TEST_CASE(GET_PARM_VIDEO_SIZE,     getParm,   g.u.videoSize,      false);
    ADD_TEST_CASE(GET_PARM_FLASH_MODE,     getParm,   g.u.flashMode,      false);
    ADD_TEST_CASE(GET_PARM_HDR_MODE,       getParm,   g.u.hdrMode,        false);
    ADD_TEST_CASE(GET_PARM_LONG_SHOT,      getParm,   g.u.longShot,       false);
    ADD_TEST_CASE(GET_PARM_LONG_EXPOSURE,  getParm,   g.u.longExposure,   false);
    ADD_TEST_CASE(GET_PARM_SMART_SHOT,     getParm,   g.u.smartShot,      false);
    ADD_TEST_CASE(GET_PARM_IMAGE_STAB,     getParm,   g.u.imageStab,      false);
    ADD_TEST_CASE(GET_PARM_VIDEO_STAB,     getParm,   g.u.videoStab,      false);
    ADD_TEST_CASE(GET_PARM_FLIP_MODE,      getParm,   g.u.flipMode,       false);
    ADD_TEST_CASE(GET_PARM_NIGHT_SHOT,     getParm,   g.u.nightShot,      false);
    ADD_TEST_CASE(GET_PARM_BEAUTY_FACE,    getParm,   g.u.beautySetting,  false);
    ADD_TEST_CASE(GET_PARM_FACE_INFO,      getParm,   g.u.faces,          false);
    ADD_TEST_CASE(GET_PARM_AE_INFO,        getParm,   g.u.ae,             false);
    ADD_TEST_CASE(GET_PARM_AWB_INFO,       getParm,   g.u.awb,            false);
    ADD_TEST_CASE(GET_PARM_AF_INFO,        getParm,   g.u.af,             false);
    ADD_TEST_CASE(GET_PARM_HDR_INFO,       getParm,   g.u.hdr,            false);
    ADD_TEST_CASE(GET_PARM_SCENE_INFO,     getParm,   g.u.scene,          false);
    ADD_TEST_CASE(GET_PARM_CAM_TYPE,       getParm,   g.u.camType,        false);
    ADD_TEST_CASE(GET_PARM_COMMAND_INF,    getParm,   g.u.commandInf,     false);
    ADD_TEST_CASE(GET_PARM_DUAL_CAM_MODE,  getParm,   g.u.dualCamMode,    false);
    ADD_TEST_CASE(GET_PARM_SUB_CAM_DATA,   getParm,   g.u.subCamData,     false);
    ADD_TEST_CASE(GET_PARM_ZOOM_INF,       getParm,   g.u.zoom,           false);
    ADD_TEST_CASE(GET_PARM_SINGLE_BOKEH,   getParm,   g.u.singleBokeh,    false);
    ADD_TEST_CASE(GET_PARM_PLATFORM_PRIV,  getParm,   g.u.platformPriv,   false);
    ADD_TEST_CASE(GET_PARM_ROTATION,       getParm,   g.u.rotation,       false);
    ADD_TEST_CASE(GET_PARM_WATER_MARK,     getParm,   g.u.watermark,      false);

    ADD_TEST_CASE(CONFIG_FOCUS_END_THRES,  getConfig, c.u.focusEnd,       true);
    ADD_TEST_CASE(CONFIG_EXPOSURE_CHANGE,  getConfig, c.u.expChange,      true);
    ADD_TEST_CASE(CONFIG_NIGHT_STAB_THRES, getConfig, c.u.nightStabThres, true);
    ADD_TEST_CASE(CONFIG_HDR_CONFI_THRES,  getConfig, c.u.hdrConfiThres,  true);
    ADD_TEST_CASE(CONFIG_EXTENDED_MSGID,   getConfig, c.u.extMsgID,       true);
    ADD_TEST_CASE(CONFIG_LOW_LIGHT_THRES,  getConfig, c.u.lowlight,       true);
    ADD_TEST_CASE(CONFIG_PARM_CATEGORY,    getConfig, c.u.parmCategory,   true);

    parm = mPalParm + getPalParmIndex(cnt, SET_PARM_MULTI_SHOT);
    parm->s.u.multiShot.num = 3;
    ADD_TEST_CASE(SET_PARM_MULTI_SHOT, setParm, s.u.multiShot, true);
    parm->s.u.multiShot.num = 1;
    ADD_TEST_CASE(SET_PARM_MULTI_SHOT, setParm, s.u.multiShot, true);

    parm = mPalParm + getPalParmIndex(cnt, SET_PARM_AE_BRACKET);
    for (uint32_t i = 0; i < ARRAYSIZE(mDefaultMultiExp); i++) {
        parm->s.u.multiExposure.add(mDefaultMultiExp[i]);
    }
    ADD_TEST_CASE(SET_PARM_AE_BRACKET, setParm, s.u.multiExposure, true);
    parm->s.u.multiExposure.clear();
    ADD_TEST_CASE(SET_PARM_AE_BRACKET, setParm, s.u.multiExposure, true);

    parm = mPalParm + getPalParmIndex(cnt, SET_PARM_PLATFORM_PRIV);
    parm->s.u.platformPriv.denoise = 1;
    parm->s.u.platformPriv.sharpen = 1;
    ADD_TEST_CASE(SET_PARM_PLATFORM_PRIV, setParm, s.u.platformPriv, true);
    parm->s.u.platformPriv.denoise = 0;
    parm->s.u.platformPriv.sharpen = 0;
    ADD_TEST_CASE(SET_PARM_PLATFORM_PRIV, setParm, s.u.platformPriv, true);

    parm = mPalParm + getPalParmIndex(cnt, OTHER_TYPE_EVT_CALLBACK);
    parm->o.u.evt.type = 0xBEEE;
    parm->o.u.evt.arg  = 0;
    for (int32_t i = 0; i < EEVT_MAX_INVALID; i++) {
        parm->o.u.evt.evt = i;
        ADD_TEST_CASE(OTHER_TYPE_EVT_CALLBACK, evtNotify, o.u.evt,  true);
    }

    parm = mPalParm + getPalParmIndex(cnt, OTHER_TYPE_DATA_CALLBACK);
    parm->o.u.data.type = 0xBEEF;
    uint32_t len  = strlen(mDefaultDataToApp);
    uint8_t data1[mDefaultDataMaxSize];
    len = len > mDefaultDataMaxSize ? mDefaultDataMaxSize : len;
    memcpy(data1, mDefaultDataToApp, len);
    parm->o.u.data.size = len;
    parm->o.u.data.data = data1;
    parm->o.u.data.release = NULL;
    ADD_TEST_CASE(OTHER_TYPE_DATA_CALLBACK, dataNotify, o.u.data, true);

    uint8_t *data2 = (uint8_t *)Malloc(len * sizeof(uint8_t));
    if (NOTNULL(data2)) {
        memcpy(data2, mDefaultDataToApp, len);
        parm->o.u.data.data = data2;
        parm->o.u.data.release = release_func;
        ADD_TEST_CASE_ASYNC(OTHER_TYPE_DATA_CALLBACK, dataNotify, o.u.data, true);
    }
    uint8_t *data3 = (uint8_t *)Malloc(len * sizeof(uint8_t));
    if (NOTNULL(data3)) {
        memcpy(data3, mDefaultDataToApp, len);
        parm->o.u.data.data = data3;
        parm->o.u.data.release = release_func;
        ADD_TEST_CASE_SYNC(OTHER_TYPE_DATA_CALLBACK, dataNotify, o.u.data, true);
    }

    return rc;
}

#define ADD_VERIFY_CASE(TYPE, ELE, CATEGORY) \
    if (SUCCEED(rc)) { \
        PalParms *parm1 = mPalParm + getPalParmIndex(lid, TYPE); \
        PalParms *parm2 = mPalParm + getPalParmIndex(rid, TYPE); \
        if (parm1->ELE != parm2->ELE) { \
            result.type   = CATEGORY; \
            result.detail = TYPE; \
            rc = JUMP_DONE; \
        } \
    }

int32_t PalTester::verifyTestResult(
    uint32_t lid, uint32_t rid, TestCaseInfo &result)
{
    int32_t rc = NO_ERROR;

    ADD_VERIFY_CASE(GET_PARM_PREVIEW_SIZE,    g.u.previewSize,    PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_PICTURE_SIZE,    g.u.pictureSize,    PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_VIDEO_SIZE,      g.u.videoSize,      PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_FLASH_MODE,      g.u.flashMode,      PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_HDR_MODE,        g.u.hdrMode,        PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_LONG_SHOT,       g.u.longShot,       PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_LONG_EXPOSURE,   g.u.longExposure,   PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_SMART_SHOT,      g.u.smartShot,      PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_IMAGE_STAB,      g.u.imageStab,      PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_VIDEO_STAB,      g.u.videoStab,      PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_FLIP_MODE,       g.u.flipMode,       PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_NIGHT_SHOT,      g.u.nightShot,      PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_BEAUTY_FACE,     g.u.beautySetting,  PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_FACE_INFO,       g.u.faces,          PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_AE_INFO,         g.u.ae,             PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_AWB_INFO,        g.u.awb,            PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_AF_INFO,         g.u.af,             PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_HDR_INFO,        g.u.hdr,            PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_SCENE_INFO,      g.u.scene,          PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_CAM_TYPE,        g.u.camType,        PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_COMMAND_INF,     g.u.commandInf,     PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_DUAL_CAM_MODE,   g.u.dualCamMode,    PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_SUB_CAM_DATA,    g.u.subCamData,     PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_ZOOM_INF,        g.u.zoom,           PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_SINGLE_BOKEH,    g.u.singleBokeh,    PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_PLATFORM_PRIV,   g.u.platformPriv,   PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_ROTATION,        g.u.rotation,       PARM_TYPE_GET);
    ADD_VERIFY_CASE(GET_PARM_WATER_MARK,      g.u.watermark,      PARM_TYPE_GET);

    ADD_VERIFY_CASE(CONFIG_FOCUS_END_THRES,   c.u.focusEnd,       PARM_TYPE_CONFIG);
    ADD_VERIFY_CASE(CONFIG_EXPOSURE_CHANGE,   c.u.expChange,      PARM_TYPE_CONFIG);
    ADD_VERIFY_CASE(CONFIG_NIGHT_STAB_THRES,  c.u.nightStabThres, PARM_TYPE_CONFIG);
    ADD_VERIFY_CASE(CONFIG_HDR_CONFI_THRES,   c.u.hdrConfiThres,  PARM_TYPE_CONFIG);
    ADD_VERIFY_CASE(CONFIG_EXTENDED_MSGID,    c.u.extMsgID,       PARM_TYPE_CONFIG);
    ADD_VERIFY_CASE(CONFIG_LOW_LIGHT_THRES,   c.u.lowlight,       PARM_TYPE_CONFIG);
    ADD_VERIFY_CASE(CONFIG_PARM_CATEGORY,     c.u.parmCategory,   PARM_TYPE_CONFIG);

    ADD_VERIFY_CASE(SET_PARM_MULTI_SHOT,      s.u.multiShot,      PARM_TYPE_SET);
    ADD_VERIFY_CASE(SET_PARM_AE_BRACKET,      s.u.multiExposure,  PARM_TYPE_SET);
    ADD_VERIFY_CASE(SET_PARM_PLATFORM_PRIV,   s.u.platformPriv,   PARM_TYPE_SET);

    ADD_VERIFY_CASE(OTHER_TYPE_EVT_CALLBACK,  o.u.evt,            PARM_TYPE_OTHERS);
    ADD_VERIFY_CASE(OTHER_TYPE_DATA_CALLBACK, o.u.data,           PARM_TYPE_OTHERS);

    return rc;
}

};

