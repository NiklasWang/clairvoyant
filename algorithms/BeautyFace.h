#ifndef ALGORITHM_BEAUTY_FACE_H_
#define ALGORITHM_BEAUTY_FACE_H_

#include "BeautyFaceTraits.h"

namespace pandora {

class  BeautyFaceArcsoft;
struct BeautyFaceParm;

class BeautyFace :
    virtual public noncopyable {

public:
    typedef BeautyFace                      MyType;
    typedef AlgTraits<MyType>::TaskType     TaskTypeT;
    typedef AlgTraits<MyType>::ResultType   ResultTypeT;
    typedef AlgTraits<MyType>::UpdateType   UpdateTypeT;
    typedef AlgTraits<MyType>::ParmType     ParmTypeT;
    typedef AlgTraits<MyType>::RequestType  RequestTypeT;
    typedef AlgTraits<MyType>::StatusType   StatusTypeT;

    AlgType getType() { return mAlgType; }
    int32_t init();
    int32_t deinit();
    int32_t process(TaskTypeT &task, ResultTypeT &result);
    int32_t process(TaskTypeT &in, TaskTypeT &out, ResultTypeT &result);
    int32_t setParm(ParmTypeT &parm);
    int32_t update(UpdateTypeT &update);
    int32_t queryCaps(AlgCaps &caps);
    int32_t queryStatus(StatusTypeT &status);

public:
    typedef GenderType Gender;

    enum GenderMask {
        GENDER_MASK_MALE        = 0x01 << GENDER_TYPE_MALE,
        GENDER_MASK_FEMALE      = 0x01 << GENDER_TYPE_FEMALE,
        GENDER_MASK_UNKNOWN     = 0x01 << GENDER_TYPE_UNKNOWN,
        GENDER_MASK_MAX_INVALID = 0x0,
    };

    enum AutoManual {
        AUTO_BEAUTY_FACE,
        MANUAL_BEAUTY_FACE,
        MAX_INVALID_BEAUTY_FACE_MODE,
    };

    enum AutoManualMask {
        AUTO_BEAUTY_FACE_MASK   = 0x01 << AUTO_BEAUTY_FACE,
        MANUAL_BEAUTY_FACE_MASK = 0x01 << MANUAL_BEAUTY_FACE,
        MODE_MASK_MAX_INVALID   = 0x0,
    };

public:
    static GenderMask getGenderMask(Gender gender);
    static AutoManualMask getAutoManualMask(AutoManual mode);
    static const char *getGenderName(Gender gender);

public:
    BeautyFace(uint32_t argNum, va_list va);
    ~BeautyFace();

private:
    AlgType  mAlgType;
    bool     mInited;
    bool     mPreviewPf;
    bool     mSnapshotPf;
    ModuleType mModule;
    uint32_t mPreviewPfCnt;
    uint32_t mSnapshotPfCnt;
    bool     mDumpPreview;
    bool     mDumpSnapshot;
    AlgCaps  mCaps;
    BeautyFaceArcsoft *mEngine;
    BeautyFaceParm    *mEngineParm;
};

};

#endif
