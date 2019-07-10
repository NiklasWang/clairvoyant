#ifndef _JPEG_SW_ENCODER_TRAITS_H_
#define _JPEG_SW_ENCODER_TRAITS_H_

#include "AlgorithmIntf.h"

namespace pandora {

struct JpegSWEncoderTaskType :
    public TaskType {
public:
    IAlgorithm *exif;

public:
    JpegSWEncoderTaskType(const TaskType &rhs) :
        TaskType(rhs) {
        exif = NULL;
    }
    JpegSWEncoderTaskType() :
        TaskType() {}
};

USE_DEFAULT_RESULT_TYPE(JpegSWEncoder);

USE_DEFAULT_PARM_TYPE(JpegSWEncoder);

USE_DEFAULT_UPDATE_TYPE(JpegSWEncoder);

USE_DEFAULT_REQUEST_TYPE(JpegSWEncoder);

USE_DEFAULT_STATUS_TYPE(JpegSWEncoder);


DEFINE_ALGORITHM_TRAITS(JpegSWEncoder);


};

#endif


