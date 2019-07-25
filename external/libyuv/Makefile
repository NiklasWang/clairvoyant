include $(MAKE_RULE)/color.print.make.rule

TARGET  = libyuv$(strip $(STLIB_EXT))
CURRDIR = $(shell pwd)

FILENAME = libyuv.a*


#exist = $(shell if [ -f $(CURRDIR)/$(TARGET) ]; then echo "exist"; else echo "notexist"; fi;)

all compile link:
	@echo -e $(NOTICE_COLOR)"This module requires following packages, make sure they are installed in your system."$(RESTORE_COLOR)
	@echo -e $(NOTICE_COLOR)" - automake, autopoint, libtool, gettext, etc."$(RESTORE_COLOR)
ifneq ($(TARGET), $(notdir $(wildcard $(CURRDIR)/$(TARGET))))
	$(call BUILD)
endif

	$(call LINUX_INSTALL)

define BUILD
	cd libyuv;   \
	make -f linux.mk
endef


define LINUX_INSTALL
	cd libyuv; \
	cp -ar ./${FILENAME} $(CURRDIR)/
endef

clean:
	cd libyuv; \
	rm -rf $(CURRDIR)/$(TARGET)* \
	rm source/*.o

.PHONY: clean
