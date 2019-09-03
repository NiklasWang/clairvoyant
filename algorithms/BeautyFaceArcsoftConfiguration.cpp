#include <time.h>

#include "BeautyFace.h"
#include "BeautyFaceArcsoft.h"

#define SUPPORTED_MAX_AGE      120
#define SUPPORTED_MANUAL_LEVEL 10

namespace pandora {

static const
struct AutoConfig {
    CameraTypeInf::CameraType cameraId;
    int32_t minAge;
    int32_t maxAge;
    int32_t genderMask;
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t>
        previewConfig[BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID];
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t>
        snapshotConfig[BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID];
} gAutoConfigs[] = {
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 0,
        .maxAge = 1,
        .genderMask = BeautyFace::GENDER_MASK_MALE | BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 1,
        .maxAge = 10,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 10,
        .maxAge = 20,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 20,
        .maxAge = 30,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 30,
        .maxAge = 40,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 40,
        .maxAge = 50,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 50,
        .maxAge = 60,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 60,
        .maxAge = 70,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 70,
        .maxAge = 80,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 80,
        .maxAge = 90,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 90,
        .maxAge = 100,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 1,
        .maxAge = 10,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 10,
        .maxAge = 20,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 20,
        .maxAge = 30,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       12, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 30,
        .maxAge = 40,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 40,
        .maxAge = 50,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 50,
        .maxAge = 60,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 60,
        .maxAge = 70,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 70,
        .maxAge = 80,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 80,
        .maxAge = 90,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 90,
        .maxAge = 100,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .minAge = 100,
        .maxAge = SUPPORTED_MAX_AGE,
        .genderMask = BeautyFace::GENDER_MASK_MALE | BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 0,
        .maxAge = 1,
        .genderMask = BeautyFace::GENDER_MASK_MALE | BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 1,
        .maxAge = 10,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 10,
        .maxAge = 20,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 20,
        .maxAge = 30,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 30,
        .maxAge = 40,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 40,
        .maxAge = 50,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 50,
        .maxAge = 60,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 60,
        .maxAge = 70,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 70,
        .maxAge = 80,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 80,
        .maxAge = 90,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 90,
        .maxAge = 100,
        .genderMask = BeautyFace::GENDER_MASK_MALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 1,
        .maxAge = 10,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 10,
        .maxAge = 20,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 20,
        .maxAge = 30,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 30,
        .maxAge = 40,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 40,
        .maxAge = 50,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 50,
        .maxAge = 60,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 60,
        .maxAge = 70,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 70,
        .maxAge = 80,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 80,
        .maxAge = 90,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 90,
        .maxAge = 100,
        .genderMask = BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .minAge = 100,
        .maxAge = SUPPORTED_MAX_AGE,
        .genderMask = BeautyFace::GENDER_MASK_MALE | BeautyFace::GENDER_MASK_FEMALE,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
};

static const
struct ShiftConfigByTime {
    CameraTypeInf::CameraType cameraId;
    int32_t  start; // Hour in one day
    int32_t  end;
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t>
        previewShift[BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID];
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t>
        snapshotShift[BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID];
} gShiftConfigByTime[] = {
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .start = 7,
        .end   = 11,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      -1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      -1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       -2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  -1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     -1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .start = 12,
        .end   = 14,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .start = 19,
        .end   = 24,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        3, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      3  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .start = 7,
        .end   = 11,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      -1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      -1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       -2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  -1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     -1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .start = 12,
        .end   = 14,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .start = 19,
        .end   = 24,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        3, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      3  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
};

static const
struct ShiftConfigByLight {
    CameraTypeInf::CameraType cameraId;
    int32_t start; // lux index
    int32_t end;
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t>
        previewShift[BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID];
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t>
        snapshotShift[BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID];
} gShiftConfigByLight[] = {
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .start = 0, // low light
        .end   = 100,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .start = 250,
        .end   = 99999,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .start = 0, // low light
        .end   = 100,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .start = 250,
        .end   = 99999,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
};

static const
struct ShiftConfigByBackLight {
    CameraTypeInf::CameraType cameraId;
    bool backlight;
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t>
        previewShift[BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID];
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t>
        snapshotShift[BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID];
} gShiftConfigByBackLight[] = {
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .backlight = false,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .backlight = true,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .backlight = false,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .backlight = true,
        .previewShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotShift = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,       5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
};

static const
struct ManualConfig {
    CameraTypeInf::CameraType cameraId;
    int32_t level;
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t>
        previewConfig[BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID];
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t>
        snapshotConfig[BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID];
} gManualConfigs[] = {
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .level = 0,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,         0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,         0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .level = 1,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   3, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  3, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      3, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .level = 2,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      8, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        4, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   6, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  6, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      6, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .level = 3,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      12, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       6, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  9, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 9, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     9, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .level = 4,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       8, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  12, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     12, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .level = 5,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      45, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      18, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .level = 6,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      28, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      50, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      50, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       12, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  18, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .level = 7,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      32, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      55, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      28, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      55, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       14, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  21, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     32, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     18, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .level = 8,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      36, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      60, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      60, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     38, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .level = 9,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      65, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      60, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  28, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     38, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     22, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_FRONT_0,
        .level = SUPPORTED_MANUAL_LEVEL,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      45, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      70, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      70, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     45, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  32, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .level = 0,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,         0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,       0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,         0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .level = 1,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        2, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   3, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      5, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  3, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      3, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .level = 2,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      8, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        4, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   6, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  6, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      6, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .level = 3,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      12, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       6, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  9, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 9, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     9, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .level = 4,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       8, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  12, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     12, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .level = 5,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      45, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      18, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .level = 6,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      28, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      50, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      50, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       12, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  18, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .level = 7,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      32, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      55, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      28, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      55, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       14, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  21, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     32, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     18, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .level = 8,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      36, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      60, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      30, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      60, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     38, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 10, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     20, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .level = 9,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      65, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      35, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      60, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  28, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     38, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     22, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
    {
        .cameraId = CameraTypeInf::CAMERA_TYPE_BACK_1,
        .level = SUPPORTED_MANUAL_LEVEL,
        .previewConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      45, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      70, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,   0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,      0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT,  0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,      0  },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        0, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
        .snapshotConfig = {
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_SOFTEN,      40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SKIN_WHITEN,      70, },
            { BeautyFaceArcsoft::BEAUTY_FACE_ANTI_SHINE,       15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TEETH_WHITEN,     45, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_ENLARGEMENT,  32, },
            { BeautyFaceArcsoft::BEAUTY_FACE_EYE_BRIGHTEN,     40, },
            { BeautyFaceArcsoft::BEAUTY_FACE_TZONE_HIGHTLIGHT, 15, },
            { BeautyFaceArcsoft::BEAUTY_FACE_SLENDER_FACE,     25, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_BLEMISH,        1, },
            { BeautyFaceArcsoft::BEAUTY_FACE_DE_POUCH,          0, },
        },
    },
};

int32_t getAutoConfig(CameraTypeInf::CameraType camId, FrameType type,
    int32_t age, BeautyFace::Gender gender, int32_t luxIndex, bool backlight,
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *result);

int32_t getManualConfig(CameraTypeInf::CameraType camId, FrameType type, int32_t level,
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *result);

int32_t runConfigTest()
{
    int32_t rc = NO_ERROR;
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem,
        int32_t> result[BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID];

    for (uint32_t t = FRAME_TYPE_PREVIEW; t < FRAME_TYPE_VIDEO; t++) {
        for (uint32_t g = GENDER_TYPE_MALE; g < GENDER_TYPE_UNKNOWN; g++) {
            for (uint32_t age = 0; age < SUPPORTED_MAX_AGE; age++) {
                CameraTypeInf::CameraType  camIds[] = {
                    CameraTypeInf::CAMERA_TYPE_FRONT_0,
                    CameraTypeInf::CAMERA_TYPE_BACK_1
                };
                for (auto camId : camIds) {
                    rc = getAutoConfig(camId, static_cast<FrameType>(t), age,
                            static_cast<BeautyFace::Gender>(g), 200, true, result);
                    ASSERT_LOG(MODULE_ALGORITHM, SUCCEED(rc),
                            "Not found auto configuration, camera type %d"
                            "type %d, gender %d, age %d, mode %s, suicide...",
                            camId, t, g, age, "auto beauty face mode");
                    break;
                }
            }
        }
    }

    for (uint32_t t = FRAME_TYPE_PREVIEW; t < FRAME_TYPE_VIDEO; t++) {
        for (int32_t i = 0; i < SUPPORTED_MANUAL_LEVEL; i++) {
            CameraTypeInf::CameraType  camIds[] = {
                CameraTypeInf::CAMERA_TYPE_FRONT_0,
                CameraTypeInf::CAMERA_TYPE_BACK_1
            };
            for (auto camId : camIds) {
                rc = getManualConfig(camId, static_cast<FrameType>(t), i, result);
                ASSERT_LOG(MODULE_ALGORITHM, SUCCEED(rc),
                        "Not found manual configuration, camera type %d"
                        "type %d mode %s level %d, suicide...",
                        camId, t, "manual beauty face mode", i);
                break;
            }
        }
    }

    return rc;
}

int32_t getShiftConfigByTime(CameraTypeInf::CameraType camId, FrameType type,
    const BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> **config)
{
    int32_t rc = NOT_FOUND;
    int32_t hour = 12;

    time_t now ;
    time(&now) ;
    struct tm tm_now ;
    localtime_r(&now, &tm_now);

    hour = tm_now.tm_hour;
    for (uint32_t i = 0; i < ARRAYSIZE(gShiftConfigByTime); i++) {
        if (camId == gShiftConfigByTime[i].cameraId &&
            hour >= gShiftConfigByTime[i].start &&
            hour < gShiftConfigByTime[i].end) {
            if (type == FRAME_TYPE_PREVIEW) {
                *config = gShiftConfigByTime[i].previewShift;
                rc = NO_ERROR;
            } else if (type == FRAME_TYPE_SNAPSHOT) {
                *config = gShiftConfigByTime[i].snapshotShift;
                rc = NO_ERROR;
            }
            break;
        }
    }

    return rc;
}

int32_t getShiftConfigByLight(CameraTypeInf::CameraType camId, int32_t luxIndex, FrameType type,
   const BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> **config)
{
    int32_t rc = NOT_FOUND;

    for (uint32_t i = 0; i < ARRAYSIZE(gShiftConfigByLight); i++) {
        if (camId == gShiftConfigByLight[i].cameraId &&
            luxIndex >= gShiftConfigByLight[i].start &&
            luxIndex < gShiftConfigByLight[i].end) {
            if (type == FRAME_TYPE_PREVIEW) {
                *config = gShiftConfigByLight[i].previewShift;
                rc = NO_ERROR;
            } else if (type == FRAME_TYPE_SNAPSHOT) {
                *config = gShiftConfigByLight[i].snapshotShift;
                rc = NO_ERROR;
            }
            break;
        }
    }

    return rc;
}

int32_t getShiftConfigByBacklight(CameraTypeInf::CameraType camId, bool isBacklight, FrameType type,
    const BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> **config)
{
    int32_t rc = NOT_FOUND;

    for (uint32_t i = 0; i < ARRAYSIZE(gShiftConfigByBackLight); i++) {
        if (camId == gShiftConfigByBackLight[i].cameraId &&
            isBacklight == gShiftConfigByBackLight[i].backlight) {
            if (type == FRAME_TYPE_PREVIEW) {
                *config = gShiftConfigByBackLight[i].previewShift;
                rc = NO_ERROR;
            } else if (type == FRAME_TYPE_SNAPSHOT) {
                *config = gShiftConfigByBackLight[i].snapshotShift;
                rc = NO_ERROR;
            }
            break;
        }
    }

    return rc;
}

int32_t getAutoDefaultConfig(CameraTypeInf::CameraType camId, FrameType type, int32_t age,
    BeautyFace::Gender gender,
    const BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> **config)
{
    int32_t rc = NOT_FOUND;

    for (uint32_t i = 0; i < ARRAYSIZE(gAutoConfigs); i++) {
        if (camId == gAutoConfigs[i].cameraId &&
            age >= gAutoConfigs[i].minAge &&
            age <  gAutoConfigs[i].maxAge &&
            BeautyFace::getGenderMask(gender) & gAutoConfigs[i].genderMask) {
            if (type == FRAME_TYPE_PREVIEW) {
                *config = gAutoConfigs[i].previewConfig;
                rc = NO_ERROR;
            } else if (type == FRAME_TYPE_SNAPSHOT) {
                *config = gAutoConfigs[i].snapshotConfig;
                rc = NO_ERROR;
            }
            break;
        }
    }

    return rc;
}

int32_t getAutoConfig(CameraTypeInf::CameraType camId, FrameType type,
    int32_t age, BeautyFace::Gender gender, int32_t luxIndex, bool backlight,
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *result)
{
    int32_t rc = NO_ERROR;
    const BeautyFaceArcsoft::ParmMap<
        BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *defaultConfig = NULL;
    const BeautyFaceArcsoft::ParmMap<
        BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *timeConfig = NULL;
    const BeautyFaceArcsoft::ParmMap<
        BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *lightConfig = NULL;
    const BeautyFaceArcsoft::ParmMap<
        BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *backlightConfig = NULL;

    if (SUCCEED(rc)) {
        rc = getAutoDefaultConfig(camId, type, age, gender, &defaultConfig);
        if (!SUCCEED(rc) || ISNULL(defaultConfig)) {
            LOGE(MODULE_ALGORITHM, "Failed to get default conf, %d", rc);
            rc = PARAM_INVALID;
        } else {
            int32_t size = BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID *
                sizeof(BeautyFaceArcsoft::ParmMap<
                    BeautyFaceArcsoft::BeautyFaceParmItem, int32_t>);
            memcpy(result, defaultConfig, size);
        }
    }

    if (SUCCEED(rc)) {
        rc = getShiftConfigByTime(camId, type, &timeConfig);
        if (!SUCCEED(rc) || ISNULL(timeConfig)) {
            LOGD(MODULE_ALGORITHM, "Failed to get time shift conf, %d", rc);
            RESETRESULT(rc);
        } else {
            for (int32_t i = 0; i < BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID; i++) {
                result[i].value += timeConfig[i].value;
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = getShiftConfigByLight(camId, luxIndex, type, &lightConfig);
        if (!SUCCEED(rc) || ISNULL(lightConfig)) {
            LOGD(MODULE_ALGORITHM, "Failed to get light shift conf, %d", rc);
            RESETRESULT(rc);
        } else {
            for (int32_t i = 0; i < BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID; i++) {
                result[i].value += lightConfig[i].value;
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = getShiftConfigByBacklight(camId, backlight, type, &backlightConfig);
        if (!SUCCEED(rc) || ISNULL(backlightConfig)) {
            LOGD(MODULE_ALGORITHM, "Failed to get backlight shift conf, %d", rc);
            RESETRESULT(rc);
        } else {
            for (int32_t i = 0; i < BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID; i++) {
                result[i].value += backlightConfig[i].value;
            }
        }
    }

    if (SUCCEED(rc)) {
        for (int32_t i = 0; i < BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID; i++) {
            result[i].value = result[i].value <   0 ?   0 : result[i].value;
            result[i].value = result[i].value > 100 ? 100 : result[i].value;
        }
    }

    return rc;
}

int32_t getManualConfig(CameraTypeInf::CameraType camId, FrameType type, int32_t level,
    BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t> *result)
{
    int32_t rc = NOT_FOUND;
    int32_t size = BeautyFaceArcsoft::BEAUTY_FACE_MAX_INVALID *
        sizeof(BeautyFaceArcsoft::ParmMap<BeautyFaceArcsoft::BeautyFaceParmItem, int32_t>);

    if (level > SUPPORTED_MANUAL_LEVEL) {
        LOGE(MODULE_OTHERS, "Unsupported level, %d/%d, reset to %d",
            level, SUPPORTED_MANUAL_LEVEL, SUPPORTED_MANUAL_LEVEL);
        level = SUPPORTED_MANUAL_LEVEL;
    }

    for (uint32_t i = 0; i < ARRAYSIZE(gManualConfigs); i++) {
        if (camId == gManualConfigs[i].cameraId &&
            level ==  gManualConfigs[i].level) {
            if (type == FRAME_TYPE_PREVIEW) {
                memcpy(result, gManualConfigs[i].previewConfig, size);
                rc = NO_ERROR;
            }
            if (type == FRAME_TYPE_SNAPSHOT) {
                memcpy(result, gManualConfigs[i].snapshotConfig, size);
                rc = NO_ERROR;
            }
            break;
        }
    }

    return rc;
}

};

