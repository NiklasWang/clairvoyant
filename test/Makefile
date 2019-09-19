CXX      = $(GLOBAL_CXX)
AR       = $(GLOBAL_AR)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS)
FFMPEGDIR = $(ROOT_DIR)/external/FFmpeg

include $(MAKE_RULE)/project.info.make.rule

TARGET      = libtest$(strip $(DYLIB_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

include $(MAKE_RULE)/find.library.make.rule

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

CXXFLAGS += -I$(ROOT_DIR) -I$(ROOT_DIR)/core -I$(ROOT_DIR)/utils -I$(ROOT_DIR)/pal/platform_ubuntu
CXXFLAGS += -I$(FFMPEGDIR)/FFmpeg/libavcodec/ -I$(FFMPEGDIR)/FFmpeg/libavformat -I$(FFMPEGDIR)/FFmpeg/libswscale


all: compile link

compile: $(objects)

link: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean