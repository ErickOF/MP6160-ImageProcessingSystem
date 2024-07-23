# Include common Makefile
include ../Makefile

SRCDIR+=../adc/src ../dac/src ../vga/src ../utils/src
INCDIR+=-I$(SYSTEMC_AMS_HOME)/include -I../adc/include -I../dac/include -I../vga/include -I../utils/include
LIBDIR+=-L$(SYSTEMC_AMS_HOME)/lib-linux64
LIBS+=-lsystemc-ams

SOURCES:=$(foreach DIR, $(SRCDIR), $(wildcard $(DIR)/*.cpp))
INCLUDES:=$(foreach DIR, $(INCDIR), $(wildcard $(DIR)/*.hpp))

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
		@gtkwave ips_ams.vcd
