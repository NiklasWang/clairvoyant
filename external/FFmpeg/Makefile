include $(MAKE_RULE)/color.print.make.rule

TARGET_LIB  = lib
CURRDIR = $(shell pwd)
TARGETS = $(CURRDIR)/FFmpeg/libswscale/libswscale.a $(CURRDIR)/FFmpeg/libavutil/libavutil.a $(CURRDIR)/FFmpeg/libavcodec/libavcodec.a $(CURRDIR)/FFmpeg/libavformat/libavformat.a 
TARGETS+= $(CURRDIR)/FFmpeg/libswresample/libswresample.a $(CURRDIR)/FFmpeg/libavdevice/libavdevice.a $(CURRDIR)/FFmpeg/libavfilter/libavfilter.a 

#exist = $(shell if [ -f $(CURRDIR)/$(TARGET) ]; then echo "exist"; else echo "notexist"; fi;)

all compile link:
	@echo -e $(NOTICE_COLOR)"This module requires following packages, make sure they are installed in your system."$(RESTORE_COLOR)
	@echo -e $(NOTICE_COLOR)" - automake, autopoint, libtool, gettext, etc."$(RESTORE_COLOR)
ifneq ($(TARGET_LIB), $(notdir $(wildcard $(CURRDIR)/lib)))
	mkdir  $(CURRDIR)/lib;
	$(call BUILD);
endif


define BUILD
	cd FFmpeg; \
	./configure; \
	make; \
	cp -f $(TARGETS) $(CURRDIR)/lib
endef


clean:
	cd FFmpeg; \
	make clean; \
	rm -rf $(CURRDIR)/lib

.PHONY: clean
