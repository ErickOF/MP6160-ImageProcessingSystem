# Adding stb lib for image handling
INCDIR_STB=lib/stb
INCLUDES_STB := $(wildcard $(INCDIR_STB)/*.h)

# Include common Makefile
include ../Makefile

# Include stb lib for compilation
INCLUDES += INCLUDES_STB

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

# Run the compiled file
run:
	@./test

# Show waveform
waveform:
	@gtkwave rgb2gray.vcd
