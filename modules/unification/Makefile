# Adding stb lib for image handling
INCDIR_STB=lib/stb
INCLUDES_STB := $(wildcard $(INCDIR_STB)/*.h)

# Include common Makefile
include ../Makefile

# Include stb lib for compilation
INCLUDES += INCLUDES_STB

# Defining preprocessor directive for using PV model
ifdef IMG_UNIFICATE_PV_EN
	CFLAGS += -DIMG_UNIFICATE_PV_EN
	LFLAGS += -DIMG_UNIFICATE_PV_EN
endif # UNIFICATE_PV_EN

# Run the compiled file
run:
	@./test

# Show waveform
waveform:
	@gtkwave UNIFICATE.vcd
