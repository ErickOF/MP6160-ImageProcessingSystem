# Include common Makefile
include ../Makefile

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

# all: encoder_testbench decoder_testbench

# encoder_testbench: tb_ethernet_encoder.o ethernetEncoder.o InputSignalGenerator.o
# 	$(CXX) -o encoder_testbench tb_ethernet_encoder.o ethernetEncoder.o InputSignalGenerator.o $(CXXFLAGS) $(LDFLAGS)

# decoder_testbench: tb_ethernet_decoder.o ethernetEncoder.o ethernetDecoder.o InputSignalGenerator.o
# 	$(CXX) -o decoder_testbench tb_ethernet_decoder.o ethernetEncoder.o ethernetDecoder.o InputSignalGenerator.o $(CXXFLAGS) $(LDFLAGS)

# tb_ethernet_encoder.o: $(SRC_DIR)/tb_ethernet_encoder.cpp
# 	$(CXX) -c $(SRC_DIR)/tb_ethernet_encoder.cpp $(CXXFLAGS) -o tb_ethernet_encoder.o

# tb_ethernet_decoder.o: $(SRC_DIR)/tb_ethernet_decoder.cpp
# 	$(CXX) -c $(SRC_DIR)/tb_ethernet_decoder.cpp $(CXXFLAGS) -o tb_ethernet_decoder.o

# ethernetEncoder.o: $(SRC_DIR)/ethernetEncoder.cpp
# 	$(CXX) -c $(SRC_DIR)/ethernetEncoder.cpp $(CXXFLAGS) -o ethernetEncoder.o

# ethernetDecoder.o: $(SRC_DIR)/ethernetDecoder.cpp
# 	$(CXX) -c $(SRC_DIR)/ethernetDecoder.cpp $(CXXFLAGS) -o ethernetDecoder.o

# InputSignalGenerator.o: $(SRC_DIR)/InputSignalGenerator.cpp
# 	$(CXX) -c $(SRC_DIR)/InputSignalGenerator.cpp $(CXXFLAGS) -o InputSignalGenerator.o

# clean:
# 	rm -f *.o encoder_testbench decoder_testbench

# Run the compiled file
run:
	@./test

# Show waveform
waveform:
	@gtkwave rgb2gray.vcd
