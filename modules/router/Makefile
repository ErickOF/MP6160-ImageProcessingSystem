# Include common Makefile
include ../Makefile

# Defining preprocessor directive for debug
ifdef IPS_DEBUG_EN
	CFLAGS += -DIPS_DEBUG_EN
	LFLAGS += -DIPS_DEBUG_EN
endif # IPS_DEBUG_EN

# Defining preprocessor directive for dumping enable
ifdef IPS_DUMP_EN
	CFLAGS += -DIPS_DUMP_EN
	LFLAGS += -DIPS_DUMP_EN
endif # IPS_DUMP_EN

# Defining preprocessor directive for normalizing the resulting magnitude
ifdef TEST_NORMALIZE_MAGNITUDE
	CFLAGS += -DTEST_NORMALIZE_MAGNITUDE
	LFLAGS += -DTEST_NORMALIZE_MAGNITUDE
endif # TEST_NORMALIZE_MAGNITUDE

# Defining preprocessor directive for using PV model
ifdef RGB2GRAY_PV_EN
	CFLAGS += -DRGB2GRAY_PV_EN
	LFLAGS += -DRGB2GRAY_PV_EN
endif # RGB2GRAY_PV_EN

ifdef EDGE_DETECTOR_PV_EN
	CFLAGS += -DEDGE_DETECTOR_PV_EN
	LFLAGS += -DEDGE_DETECTOR_PV_EN
endif # EDGE_DETECTOR_PV_EN

# Defining preprocessor directive for using PV model
ifdef EDGE_DETECTOR_LT_EN
	CFLAGS += -DEDGE_DETECTOR_LT_EN
	LFLAGS += -DEDGE_DETECTOR_LT_EN
endif # EDGE_DETECTOR_LT_EN

# Defining preprocessor directive for using PV model
ifdef EDGE_DETECTOR_AT_EN
	CFLAGS += -DEDGE_DETECTOR_AT_EN
	LFLAGS += -DEDGE_DETECTOR_AT_EN
endif # EDGE_DETECTOR_AT_EN

ifdef USING_TLM_TB_EN
	CFLAGS += -DUSING_TLM_TB_EN
	LFLAGS += -DUSING_TLM_TB_EN
endif # USING_TLM_TB_EN

ifdef IPS_FILTER_LT_EN
	CFLAGS += -DIPS_FILTER_LT_EN
	LFLAGS += -DIPS_FILTER_LT_EN
endif # IPS_FILTER_LT_EN

ifdef IMG_UNIFICATE_PV_EN
	CFLAGS += -DIMG_UNIFICATE_PV_EN
	LFLAGS += -DIMG_UNIFICATE_PV_EN
endif # UNIFICATE_PV_EN

ifdef USING_TLM_ROUTER_TB_EN
	CFLAGS += -DUSING_TLM_ROUTER_TB_EN
	LFLAGS += -DUSING_TLM_ROUTER_TB_EN
endif # USING_TLM_TB_EN

ifdef IPS_JPG_PV_EN
	CFLAGS += -DIPS_JPG_PV_EN
	LFLAGS += -DIPS_JPG_PV_EN
endif # IPS_JPG_PV_EN

ifdef DISABLE_ROUTER_DEBUG
	CFLAGS += -DDISABLE_ROUTER_DEBUG
	LFLAGS += -DDISABLE_ROUTER_DEBUG
endif # DISABLE_ROUTER_DEBUG

ifdef DISABLE_FILTER_DEBUG
	CFLAGS += -DDISABLE_FILTER_DEBUG
	LFLAGS += -DDISABLE_FILTER_DEBUG
endif # DISABLE_FILTER_DEBUG

ifdef DISABLE_MEM_DEBUG
	CFLAGS += -DDISABLE_MEM_DEBUG
	LFLAGS += -DDISABLE_MEM_DEBUG
endif # DISABLE_MEM_DEBUG

ifdef DISABLE_RGB_DEBUG
	CFLAGS += -DDISABLE_RGB_DEBUG
	LFLAGS += -DDISABLE_RGB_DEBUG
endif # DISABLE_RGB_DEBUG

ifdef DISABLE_SOBEL_DEBUG
	CFLAGS += -DDISABLE_SOBEL_DEBUG
	LFLAGS += -DDISABLE_SOBEL_DEBUG
endif # DISABLE_SOBEL_DEBUG

ifdef DISABLE_TB_DEBUG
	CFLAGS += -DDISABLE_TB_DEBUG
	LFLAGS += -DDISABLE_TB_DEBUG
endif # DISABLE_TB_DEBUG

.PHONY: print-all
print-all:
		@echo "Incdir is $(INCDIR)"
		@echo "Srcdir is $(SRCDIR)"
		@echo "Srcdirs are $(SRCDIRS)"
		@echo "Includes are $(INCLUDES)"
		@echo "Sources are $(SOURCES)"
		@echo "Objects are $(OBJECTS)"

.PHONY: print-flags
print-flags:
		@echo "Flags are $(CFLAGS)"

# Run the compiled file
run:
	@./test

# Show waveform
waveform:
	@gtkwave router.vcd
