CC = gcc
CFLAGS = -std=c99 -DBUILD_LINUX_X86_64 -I$(ROOT_DIR)/external/freetype/freetype-2.9.1/include
LDFLAGS = -L$(ROOT_DIR)/external/freetype/freetype-2.9.1/objs/.libs -lfreetype
TARGET = font_tester
sources = FontBitmapTest.c
objects = $(sources:.c=.o)
dependence:=$(sources:.c=.d)

all: $(objects)
	$(CC) $^ $(LDFLAGS) -o $(TARGET)

include $(dependence)

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY: clean

clean:
	rm -f $(TARGET) $(objects) $(dependence)

