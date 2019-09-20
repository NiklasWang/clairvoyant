CXX      = $(GLOBAL_CXX)
AR       = $(GLOBAL_AR)
CXXFLAGS = $(GLOBAL_CXXFLAGS)
LDFLAGS  = $(GLOBAL_LDFLAGS) -pthread
FFMPEGDIR = $(ROOT_DIR)/external/FFmpeg/FFmpeg

include $(MAKE_RULE)/project.info.make.rule

TARGET      = $(PROJNAME)$(strip $(EXE_EXT))
sources     = $(wildcard *.cpp)
objects     = $(sources:.cpp=.o)
dependence := $(sources:.cpp=.d)

include $(MAKE_RULE)/find.all.modules.make.rule

SHARED_LIBS = $(DYLIB_MODULE)
STATIC_LIBS = $(STLIB_MODULE)

include $(MAKE_RULE)/find.library.make.rule 

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

LDFLAGS += -L $(ROOT_DIR)/external/freetype/freetype-2.6/objs/libs -L $(ROOT_DIR)/external/libjpeg-turbo/libjpeg-turbo/libs -L $(ROOT_DIR)/external/zlib/zlib -I/usr/include/harfbuzz  -I/usr/include/libpng  -Wl,-Bstatic  -lfreetype -lturbojpeg -lpng -lexif -lutils -lutil -Wl,-Bdynamic -llog -lharfbuzz -lz

CXXFLAGS += -I$(ROOT_DIR)/test/
LDFLAGS += -L $(ROOT_DIR)/test/

CXXFLAGS   +=  -I$(ROOT_DIR) -I$(ROOT_DIR)/core -I$(ROOT_DIR)/utils -I$(ROOT_DIR)/pal/platform_ubuntu
CXXFLAGS   +=  -I$(FFMPEGDIR)/libavcodec/ -I$(FFMPEGDIR)/libavformat -I$(FFMPEGDIR)/libswscale -I$(FFMPEGDIR)/libavdevice -I$(FFMPEGDIR)/libavutil -I$(ROOT_DIR)/external/SDL/SDL2/include
LDFLAGS    +=  -L $(ROOT_DIR)/external/zlib -L $(FFMPEGDIR)/../lib  -L /usr/lib/x86_64-linux-gnu
LDFLAGS    +=  -Bstatic -lavformat -lavcodec -lavutil -lswscale -lswresample -lz -llzma -lavdevice -lavfilter -lSDL2 -lxcb-shm -lxcb -lxcb-xfixes -lxcb-shape -ldl -lasound

all: compile link

compile: $(objects)

link: $(objects)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean