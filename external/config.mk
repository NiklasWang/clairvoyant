# Compile configuration for vendor libraries

# Switches
LIBRARY_YUV   := y
LIBRARY_EXIV2 := n
LIB_FREE_TYPE := y
LIB_TURO_JPEG := y
LIB_JPEG_EXIF := y

# Vars
SRC_FILE_PATH            := external
EXTERNAL_INCLUDES_PATH   := external

# Files to compile for each algorithms
COMPILE-$(LIBRARY_YUV)   :=
COMPILE-$(LIBRARY_EXIV2) +=
COMPILE-$(LIB_FREE_TYPE) +=
COMPILE-$(LIB_TURO_JPEG) +=
COMPILE-$(LIB_JPEG_EXIF) +=

# C flags for each external libraries
CFLAGS-$(LIBRARY_YUV)    +=
CFLAGS-$(LIBRARY_EXIV2)  +=
CFLAGS-$(LIB_FREE_TYPE)  +=
CFLAGS-$(LIB_TURO_JPEG)  +=
CFLAGS-$(LIB_JPEG_EXIF)  +=

# C includes for each external libraries
CINCLUDES-$(LIBRARY_YUV)   += libyuv/include
CINCLUDES-$(LIBRARY_EXIV2) += libexiv2/exiv2-trunk/include
CINCLUDES-$(LIB_FREE_TYPE) += ../../../../../../external/freetype/include
CINCLUDES-$(LIB_TURO_JPEG) += ../../../../../../external/libjpeg-turbo
CINCLUDES-$(LIB_JPEG_EXIF) += ../../../../../../external/libexif/libexif

# External shared library to link with
LIBRARY-$(LIBRARY_YUV)   +=
LIBRARY-$(LIBRARY_EXIV2) +=
LIBRARY-$(LIB_FREE_TYPE) += libft2
LIBRARY-$(LIB_TURO_JPEG) += libturbojpeg
LIBRARY-$(LIB_JPEG_EXIF) += libexif

# External static library to link
STATICLIB-$(LIBRARY_YUV)   += libpandora_yuv
STATICLIB-$(LIBRARY_EXIV2) += libpandora_exiv2 libpandora_expat
STATICLIB-$(LIB_FREE_TYPE) +=
STATICLIB-$(LIB_TURO_JPEG) +=
STATICLIB-$(LIB_JPEG_EXIF) +=

# Add prefix for all vars
COMPILE-y   := $(patsubst %, $(SRC_FILE_PATH)/%.cpp, $(sort $(COMPILE-y)))
CFLAGS-y    := $(sort $(CFLAGS-y))
CINCLUDES-y := $(patsubst %, $(EXTERNAL_INCLUDES_PATH)/%, $(sort $(CINCLUDES-y)))
LIBRARY-y   := $(sort $(LIBRARY-y))
STATICLIB-y := $(sort $(STATICLIB-y))
COMPILE-Y   := $(COMPILE-y)
CFLAGS-Y    := $(CFLAGS-y)
CINCLUDES-Y := $(CINCLUDES-y)
LIBRARY-Y   := $(LIBRARY-y)
STATICLIB-Y := $(STATICLIB-y)
