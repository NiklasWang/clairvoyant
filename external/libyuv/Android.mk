# This is the Android makefile for libyuv for both platform and NDK.

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

YUV_TARGET_ARCH_ABI := armeabi-v7a

LOCAL_CPP_EXTENSION := .cc

LOCAL_SRC_FILES := \
    source/compare.cc           \
    source/compare_common.cc    \
    source/convert.cc           \
    source/convert_argb.cc      \
    source/convert_from.cc      \
    source/convert_from_argb.cc \
    source/convert_to_argb.cc   \
    source/convert_to_i420.cc   \
    source/cpu_id.cc            \
    source/planar_functions.cc  \
    source/rotate.cc            \
    source/rotate_any.cc        \
    source/rotate_argb.cc       \
    source/rotate_common.cc     \
    source/row_any.cc           \
    source/row_common.cc        \
    source/scale.cc             \
    source/scale_any.cc         \
    source/scale_argb.cc        \
    source/scale_common.cc      \
    source/video_common.cc

ifeq ($(YUV_TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_CFLAGS += -DLIBYUV_NEON
    LOCAL_SRC_FILES += \
        source/compare_neon.cc    \
        source/rotate_neon.cc     \
        source/row_neon.cc        \
        source/scale_neon.cc
endif

ifeq ($(YUV_TARGET_ARCH_ABI),arm64-v8a)
    LOCAL_CFLAGS += -DLIBYUV_NEON
    LOCAL_SRC_FILES += \
        source/compare_neon64.cc    \
        source/rotate_neon64.cc     \
        source/row_neon64.cc        \
        source/scale_neon64.cc 
endif

ifeq ($(YUV_TARGET_ARCH_ABI),$(filter $(YUV_TARGET_ARCH_ABI), x86 x86_64))
    LOCAL_SRC_FILES += \
        source/compare_gcc.cc       \
        source/rotate_gcc.cc        \
        source/row_gcc.cc           \
        source/scale_gcc.cc
endif


LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/include

LOCAL_MODULE := libpandora_yuv
LOCAL_MODULE_TAGS := optional

include $(BUILD_STATIC_LIBRARY)

