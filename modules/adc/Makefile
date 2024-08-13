# Include common Makefile
include ../Makefile

SRCDIR+=../../utils/src
INCDIR+=-I$(SYSTEMC_AMS_HOME)/include -I../utils/include
LIBDIR+=-L$(SYSTEMC_AMS_HOME)/lib-linux64
LIBS+=-lsystemc-ams

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

# Run the compiled file
run:
		@./$(TARGET)

# Show waveform
waveform:
		@gtkwave ips_adc.vcd
