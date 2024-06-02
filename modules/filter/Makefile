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

# Defining preprocessor directive for test mode one window random
ifdef TEST_MODE_ONE_WINDOW_RANDOM
	CFLAGS += -DTEST_MODE_ONE_WINDOW -DTEST_MODE_ONE_WINDOW_RANDOM
	LFLAGS += -DTEST_MODE_ONE_WINDOW -DTEST_MODE_ONE_WINDOW_RANDOM
endif # TEST_MODE_ONE_WINDOW_RANDOM

# Defining preprocessor directive for test mode one wildcard normal
ifdef TEST_MODE_ONE_WINDOW_NORMAL
	CFLAGS += -DTEST_MODE_ONE_WINDOW -DTEST_MODE_ONE_WINDOW_NORMAL
	LFLAGS += -DTEST_MODE_ONE_WINDOW -DTEST_MODE_ONE_WINDOW_NORMAL
endif # TEST_MODE_ONE_WINDOW_NORMAL


# Run the compiled file
run:
	@./test

# Show waveform
waveform:
	@gtkwave ips_filter.vcd
