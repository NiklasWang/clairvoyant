LOCAL_PATH    := $(call my-dir)
PANDORA_PATH  := $(LOCAL_PATH)/..
EXTERNAL_PATH := $(LOCAL_PATH)

include $(EXTERNAL_PATH)/config.mk

ifneq "$(findstring libpandora_yuv, $(STATICLIB-Y))" ""
  include $(EXTERNAL_PATH)/libyuv/Android.mk
endif

ifneq "$(findstring libpandora_exiv2, $(STATICLIB-Y))" ""
  include $(EXTERNAL_PATH)/libexiv2/Android.mk
endif

ifneq "$(findstring libft2, $(LIBRARY-Y))" ""
  # Use system rules
endif

ifneq "$(findstring libturbojpeg, $(LIBRARY-y))" ""
  include $(CLEAR_VARS)
  LOCAL_MODULE        := libturbojpeg
  LOCAL_MODULE_CLASS  := SHARED_LIBRARIES
  LOCAL_MODULE_SUFFIX := .so
  LOCAL_MULTILIB      := 32
  LOCAL_STRIP_MODULE  := false
  LOCAL_MODULE_OWNER  := qcom
  LOCAL_MODULE_TAGS   := optional
  LOCAL_SRC_FILES     := ../lib/libjpeg-turbo/libturbojpeg.so
  LOCAL_MODULE_PATH   := $(PRODUCT_OUT)/vendor/lib
  include $(BUILD_PREBUILT)
endif

ifneq "$(findstring libexif, $(LIBRARY-y))" ""
  # Use system rules
endif
