LOCAL_PATH := $(call my-dir)
SIRIUS_CLIENT_PATH := $(PANDORA_PATH)/../Sirius

include $(CLEAR_VARS)

LOCAL_SRC_FILES   :=               \
    SiriusClientImpl.cpp

LOCAL_C_INCLUDES  :=               \
    $(SIRIUS_CLIENT_PATH)          \
    $(SIRIUS_CLIENT_PATH)/utils    \
    $(SIRIUS_CLIENT_PATH)/utils/sp \
    $(SIRIUS_CLIENT_PATH)/log      \
    $(SIRIUS_CLIENT_PATH)/core     \
    $(SIRIUS_CLIENT_PATH)/threads  \
    $(SIRIUS_CLIENT_PATH)/memory   \
    $(SIRIUS_CLIENT_PATH)/socket   \
    $(SIRIUS_CLIENT_PATH)/buffer   \
    $(SIRIUS_CLIENT_PATH)/usecase  \
    $(PANDORA_PATH)                \
    $(PANDORA_PATH)/utils          \
    $(PANDORA_PATH)/core

LOCAL_SHARED_LIBRARIES := libsirius
LOCAL_CFLAGS      := -Werror
LOCAL_CFLAGS      += -DENABLE_LOGGER -DPRINT_LOGCAT_LOG -DMEMORY_POOL_MODE
LOCAL_MODULE_TAGS := optional
LOCAL_32_BIT_ONLY := $(BOARD_QTI_CAMERA_32BIT_ONLY)
LOCAL_MODULE      := libsirius_client

include $(BUILD_STATIC_LIBRARY)
