APPL_COBJS += cli_main.o apploader.o gui.o

COPTS += -DBUILD_LOADER
BUILD_LOADER=1

ifdef CONFIG_EV3RT_APPLICATION

include $(APPLDIRS)/zmodem/Makefile
include $(TARGETDIR)/dmloader/Makefile
include $(TARGETDIR)/drivers/mbed/Makefile

endif

