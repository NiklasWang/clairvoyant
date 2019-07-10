#ifndef ALGORITHM_FONT_WATER_MARK_H_
#define ALGORITHM_FONT_WATER_MARK_H_

#include <vector>
#include <string>

#include "FontWaterMarkTraits.h"
#include "Font2Yuv.h"

#define __FUNCTION_TEST_

namespace pandora {

struct FontBlockCoordinate;

class FontWaterMark :
    virtual public noncopyable {

public:
    typedef FontWaterMark                   MyType;
    typedef AlgTraits<MyType>::TaskType     TaskTypeT;
    typedef AlgTraits<MyType>::ResultType   ResultTypeT;
    typedef AlgTraits<MyType>::UpdateType   UpdateTypeT;
    typedef AlgTraits<MyType>::ParmType     ParmTypeT;
    typedef AlgTraits<MyType>::RequestType  RequestTypeT;
    typedef AlgTraits<MyType>::StatusType   StatusTypeT;

    AlgType getType() { return ALG_UTILS; }
    int32_t init();
    int32_t deinit();
    int32_t process(TaskTypeT &task, ResultTypeT &result);
    int32_t process(TaskTypeT &in, TaskTypeT &out, ResultTypeT &result);
    int32_t setParm(ParmTypeT &parm);
    int32_t update(UpdateTypeT &update);
    int32_t queryCaps(AlgCaps &caps);
    int32_t queryStatus(StatusTypeT &status);

public:
    FontWaterMark(uint32_t argNum, va_list va);
    ~FontWaterMark();

private:
    int32_t getFontNum();
    int32_t buildFontMap();
    int32_t buildFontYuv(int32_t yuvW, int32_t yuvH);
    int32_t calcFontWaterMarkSize(int32_t *outputW, int32_t *outputH);
    int32_t calcFontBlockStartingCoordinates();

private:
    bool     mInited;
    ModuleType mModule;
    bool     mPf;
    uint32_t mPfCnt;
    bool     mDump;
    AlgCaps  mCaps;

private:
    font_bitmap_t *mFontMap;
    uint8_t       *mFontYuv;
    FontBlockCoordinate     *mFontBlockXY;
    int32_t        mYuvW;
    int32_t        mYuvH;
    std::vector<std::string> mStrings;
    std::vector<int32_t>     mSizes;
    sp<IAlgorithm> mRotator;
    RotationAngle  mRotation;
    FontWaterMarkPosition    mPosition;
    bool mNeedMirror;

#ifdef __FUNCTION_TEST_
public:
    int32_t testNewInterface(int32_t rc);
#endif
};

};

#endif

