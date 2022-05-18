#-------------------------------------------------------------------------------
.SUFFIXES:
#-------------------------------------------------------------------------------

export SOH_TOP_DIR := $(CURDIR)

.PHONY: all clean ZAPDUtils libultraship soh

all: soh
	@echo "Done!"

ZAPDUtils:
	@echo "Building $@..."
	@$(MAKE) --no-print-directory -C $(CURDIR)/ZAPDTR/ZAPDUtils -f $(CURDIR)/ZAPDTR/ZAPDUtils/Makefile.switch

libultraship: ZAPDUtils
	@echo "Building $@..."
	@$(MAKE) --no-print-directory -C $(CURDIR)/libultraship -f $(CURDIR)/libultraship/Makefile.switch

soh: libultraship
	@echo "Building $@..."
	@$(MAKE) --no-print-directory -C $(CURDIR)/soh -f $(CURDIR)/soh/Makefile.switch

clean:
	@$(MAKE) --no-print-directory -C $(CURDIR)/ZAPDTR/ZAPDUtils -f $(CURDIR)/ZAPDTR/ZAPDUtils/Makefile.switch clean
	@$(MAKE) --no-print-directory -C $(CURDIR)/libultraship -f $(CURDIR)/libultraship/Makefile.switch clean
	@$(MAKE) --no-print-directory -C $(CURDIR)/soh -f $(CURDIR)/soh/Makefile.switch clean