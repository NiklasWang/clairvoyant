LOCAL_PATH := $(call my-dir)
PANDORA_PATH  := $(LOCAL_PATH)

include $(CLEAR_VARS)

LOCAL_SRC_FILES :=               \
    core/Pandora.cpp             \
    core/PandoraSingleton.cpp    \
    core/PandoraImpl.cpp         \
    core/PandoraIntfInternal.cpp \
    core/StatusContainer.cpp     \
    core/CameraStatus.cpp        \
    core/CameraStatusMgr.cpp     \
    core/PandoraParameters.cpp   \
    core/PandoraCore.cpp         \
    core/PandoraCoreTester.cpp   \
    core/AlgorithmInstance.cpp   \
    core/AlgorithmIntf.cpp       \
    core/AlgorithmType.cpp       \
    core/AlgorithmHelper.cpp     \
    core/AlgorithmProperties.cpp \
    utils/Modules.cpp            \
    utils/QueueC.cpp             \
    utils/Semaphore.cpp          \
    utils/SemaphoreTimeout.cpp   \
    utils/SyncType.cpp           \
    utils/PfLogger.cpp           \
    utils/PfLoggerImpl.cpp       \
    utils/Identifier.cpp         \
    utils/FDTracker.cpp          \
    utils/FDTrackerImpl.cpp      \
    utils/FrameReorder.cpp       \
    utils/FrameCounter.cpp       \
    utils/Logo.cpp               \
    utils/Watermark.cpp          \
    utils/sp/RefBase.cpp         \
    memory/PoolImpl.cpp          \
    memory/MemMgmt.cpp           \
    threads/Thread.cpp           \
    threads/ThreadPool.cpp       \
    threads/ThreadPoolEx.cpp     \
    threads/ThreadRunner.cpp     \
    pal/Pal.cpp                  \
    pal/PalImpl.cpp              \
    pal/PalParms.cpp             \
    pal/PlatformOpsIntf.cpp      \
    pal/ConfigParameters.cpp     \
    pal/PalTester.cpp            \
    algorithms/utils/YUVRotator.cpp    \
    algorithms/utils/YUVScaler.cpp     \
    algorithms/utils/BmpWaterMark.cpp  \
    algorithms/utils/Bmp2Yuv.cpp       \
    algorithms/utils/FontWaterMark.cpp \
    algorithms/utils/Font2Yuv.cpp      \
    algorithms/utils/JpegSWEncoder.cpp \
    algorithms/utils/JpegExif.cpp      \
    algorithms/WaterMark.cpp           \
    log/LogImpl.cpp                    \
    sirius/SiriusClient.cpp


LOCAL_C_INCLUDES :=                \
    $(LOCAL_PATH)/core             \
    $(LOCAL_PATH)/utils            \
    $(LOCAL_PATH)/utils/sp         \
    $(LOCAL_PATH)/algorithms       \
    $(LOCAL_PATH)/algorithms/utils \
    $(LOCAL_PATH)/memory           \
    $(LOCAL_PATH)/threads          \
    $(LOCAL_PATH)/pal              \
    $(LOCAL_PATH)/log              \
    $(LOCAL_PATH)/external

LOCAL_CFLAGS  := -std=c++11 -frtti
LOCAL_CFLAGS  += -Wall -Wextra -Werror
LOCAL_CFLAGS  += -DENABLE_LOGGER -DPRINT_LOGCAT_LOG -DMEMORY_POOL_MODE

LOCAL_LDFLAGS +=

LOCAL_SHARED_LIBRARIES += liblog

LOCAL_CPP_FEATURES := rtti #exceptions

include $(PANDORA_PATH)/external/config.mk
LOCAL_SRC_FILES        += $(COMPILE-Y)
LOCAL_C_INCLUDES       += $(patsubst %, $(LOCAL_PATH)/%, $(CINCLUDES-Y))
LOCAL_CFLAGS           += $(CFLAGS-Y)
LOCAL_SHARED_LIBRARIES += $(LIBRARY-Y)
LOCAL_STATIC_LIBRARIES += $(STATICLIB-Y)

include $(PANDORA_PATH)/vendor/config.mk
LOCAL_SRC_FILES        += $(COMPILE-Y)
LOCAL_C_INCLUDES       += $(patsubst %, $(LOCAL_PATH)/%, $(CINCLUDES-Y))
LOCAL_CFLAGS           += $(CFLAGS-Y)
LOCAL_SHARED_LIBRARIES += $(LIBRARY-Y)
LOCAL_STATIC_LIBRARIES += $(STATICLIB-Y)

COMPILE_WITH_SIRIUS    := yes

ifeq ($(COMPILE_WITH_SIRIUS),yes)
LOCAL_C_INCLUDES       += $(LOCAL_PATH)/sirius
LOCAL_CFLAGS           += -DENABLE_SIRIUS
LOCAL_STATIC_LIBRARIES += libsirius_client
LOCAL_SHARED_LIBRARIES += libsirius
endif

LOCAL_VENDOR_MODULE := true

LOCAL_32_BIT_ONLY := $(BOARD_QTI_CAMERA_32BIT_ONLY)

LOCAL_MODULE := libpandora

include $(BUILD_SHARED_LIBRARY)

include $(PANDORA_PATH)/external/Android.mk
include $(PANDORA_PATH)/vendor/Android.mk
#include $(PANDORA_PATH)/tools/Android.mk

ifeq ($(COMPILE_WITH_SIRIUS),yes)
include $(PANDORA_PATH)/sirius/Android.mk
endif

