CC      = $(GLOBAL_CC)
CFLAGS  = $(GLOBAL_CFLAGS) -DBUILD_LINUX_X86_64 -I$(ROOT_DIR)/external/freetype/freetype-2.9.1/include
LDFLAGS = $(GLOBAL_LDFLAGS) -L$(ROOT_DIR)/external/freetype/freetype-2.9.1/objs/.libs -lfreetype

TARGET      = font_tester$(strip $(EXE_EXT))
sources     = FontBitmapTest.c
objects     = $(sources:.c=.o)
dependence := $(sources:.c=.d)

-include $(dependence)

include $(MAKE_RULE)/dependency.make.rule

all: compile link

compile: $(objects)

link: $(objects)
	$(CC) $^ $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(objects) $(dependence)

.PHONY: clean
