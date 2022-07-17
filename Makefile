#-------------------------------------------------------------------------------
.SUFFIXES:
#-------------------------------------------------------------------------------

export SOH_TOP_DIR := $(CURDIR)

.PHONY: all clean ZAPDUtils libultraship soh StormLib

all: soh
	@echo "Done!"

ZAPDUtils:
	@echo "Building $@..."
	@$(MAKE) --no-print-directory -C $(CURDIR)/ZAPDTR/ZAPDUtils -f $(CURDIR)/ZAPDTR/ZAPDUtils/Makefile.switch

StormLib:
	@echo "Building $@..."
	LDFLAGS="" ${DEVKITPRO}/portlibs/switch/bin/aarch64-none-elf-cmake -DCMAKE_TOOLCHAIN_FILE=${DEVKITPRO}/cmake/Switch.cmake -DCMAKE_BUILD_TYPE=Release -B $(CURDIR)/StormLib/nxbuild -S $(CURDIR)/StormLib
	@$(MAKE) --no-print-directory -C $(CURDIR)/StormLib/nxbuild -f $(CURDIR)/StormLib/nxbuild/Makefile

libultraship: StormLib ZAPDUtils
	@echo "Building $@..."
	@$(MAKE) --no-print-directory -C $(CURDIR)/libultraship -f $(CURDIR)/libultraship/Makefile.switch

soh: libultraship
	@echo "Building $@..."
	@$(MAKE) --no-print-directory -C $(CURDIR)/soh -f $(CURDIR)/soh/Makefile.switch

otr:
	@echo "Building $@..."
	@$(MAKE) --no-print-directory -C $(CURDIR)/soh -f $(CURDIR)/soh/Makefile setup

clean:
	@$(MAKE) --no-print-directory -C $(CURDIR)/ZAPDTR/ZAPDUtils -f $(CURDIR)/ZAPDTR/ZAPDUtils/Makefile.switch clean
	@$(MAKE) --no-print-directory -C $(CURDIR)/libultraship -f $(CURDIR)/libultraship/Makefile.switch clean
	@$(MAKE) --no-print-directory -C $(CURDIR)/soh -f $(CURDIR)/soh/Makefile.switch clean