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

# Defining preprocessor directive for test modes
ifdef TEST_MODE_IMAGE
		CFLAGS += -DTEST_MODE_IMAGE
		LFLAGS += -DTEST_MODE_IMAGE
else
ifdef TEST_MODE_ONE_WINDOW_RANDOM
		CFLAGS += -DTEST_MODE_ONE_WINDOW -DTEST_MODE_ONE_WINDOW_RANDOM
		LFLAGS += -DTEST_MODE_ONE_WINDOW -DTEST_MODE_ONE_WINDOW_RANDOM
else
ifdef TEST_MODE_ONE_WINDOW_NORMAL
		CFLAGS += -DTEST_MODE_ONE_WINDOW -DTEST_MODE_ONE_WINDOW_NORMAL
		LFLAGS += -DTEST_MODE_ONE_WINDOW -DTEST_MODE_ONE_WINDOW_NORMAL
endif # TEST_MODE_ONE_WINDOW_NORMAL
endif # TEST_MODE_ONE_WINDOW_RANDOM
endif # TEST_MODE_IMAGE

# Defining preprocessor directive for model - by default PV model
ifdef IPS_FILTER_LT_EN
		CFLAGS += -DIPS_FILTER_LT_EN
		LFLAGS += -DIPS_FILTER_LT_EN
else # IPS_FILTER_AT_EN
ifdef IPS_FILTER_AT_EN
		CFLAGS += -DIPS_FILTER_AT_EN
		LFLAGS += -DIPS_FILTER_AT_EN
else # IPS_FILTER_CA_EN
ifdef IPS_FILTER_CA_EN
		CFLAGS += -DIPS_FILTER_CA_EN
		LFLAGS += -DIPS_FILTER_CA_EN
else # IPS_FILTER_PV_EN
		CFLAGS += -DIPS_FILTER_PV_EN
		LFLAGS += -DIPS_FILTER_PV_EN
endif # IPS_FILTER_CA_EN
endif # IPS_FILTER_AT_EN
endif # IPS_FILTER_LT_EN

# Run the compiled file
run:
		@./test

# Show waveform
waveform:
		@gtkwave ips_filter.vcd
