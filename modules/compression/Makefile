# Include common Makefile
include ../Makefile

ifdef IPS_JPG_PV_EN
	CFLAGS += -DIPS_JPG_PV_EN
	LFLAGS += -DIPS_JPG_PV_EN
endif # IPS_JPG_PV_EN

ifdef IPS_JPG_LT_EN
	CFLAGS += -DIPS_JPG_LT_EN
	LFLAGS += -DIPS_JPG_LT_EN
endif # IPS_JPG_LT_EN

ifdef IPS_JPG_AT_EN
	CFLAGS += -DIPS_JPG_AT_EN
	LFLAGS += -DIPS_JPG_AT_EN
endif # IPS_JPG_AT_EN

# Run the compiled file
run:
	@./$(TARGET)

# Show waveform
waveform:
	@gtkwave jpeg_compressor.vcd
