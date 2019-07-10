CC = gcc
CFLAGS = -std=c99 -DBUILD_LINUX_X86_64 -I/usr/include/freetype2/freetype -I/usr/include/freetype2
LDFLAGS = -L/usr/lib/x86_64-linux-gnu/ -lfreetype
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

