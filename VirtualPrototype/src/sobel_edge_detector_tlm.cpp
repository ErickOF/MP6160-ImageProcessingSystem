#ifndef SOBEL_EDGE_DETECTOR_TLM_CPP
#define SOBEL_EDGE_DETECTOR_TLM_CPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include "sobel_edge_detector_tlm.hpp"
#include "address_map.hpp"

#include "common_func.hpp"

void sobel_edge_detector_tlm::do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address){
  short int sobel_results[4];
  sc_int<16> local_result;

  Edge_Detector::address = (sc_uint<24>) address+SOBEL_OUTPUT_ADDRESS_LO;
  read();
  
  for (int i = 0; i < data_length; i++)
  {
    local_result = Edge_Detector::data.range((address + i + 1) * 16 - 1, (address+i) * 16).to_int();
    sobel_results[address+i] = (short int)local_result;
      *(data+i) = sobel_results[address+i];
    //dbgimgtarmodprint("%0d", sobel_results[address+i]);
  }
  
}

void sobel_edge_detector_tlm::do_when_write_transaction(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  if (address < SOBEL_INPUT_0_SIZE) {
    for (int i = 0; i < data_length; data++) {
      this->sobel_input[address+i] = *(data+i);
    }
  }
  else if (SOBEL_INPUT_0_SIZE <= address && address < SOBEL_INPUT_0_SIZE + SOBEL_INPUT_1_SIZE) {
     for (int i = 0; i < data_length; data++) {
      this->sobel_input[address+i-SOBEL_INPUT_0_SIZE] = *(data+i-SOBEL_INPUT_0_SIZE);
    }
  }

  Edge_Detector::data = (this->sobel_input[7], this->sobel_input[6], this->sobel_input[5], this->sobel_input[4], this->sobel_input[3], this->sobel_input[2], this->sobel_input[1], this->sobel_input[0]);
  Edge_Detector::address = address+SOBEL_INPUT_0_ADDRESS_LO;

  if ((address == SOBEL_INPUT_0_SIZE-1) || (address == SOBEL_INPUT_0_SIZE+SOBEL_INPUT_1_SIZE-1)) {
    write();
  }
  
}

void sobel_edge_detector_tlm::read()
{
  if ((Edge_Detector::address - SOBEL_OUTPUT_ADDRESS_LO) == 0)
  {
    Edge_Detector::data = (sc_uint<32>(0), resultSobelGradientY, resultSobelGradientX);
  }
}

void sobel_edge_detector_tlm::write()
{
  wr_t.notify(0, SC_NS);
}

#endif // SOBEL_EDGE_DETECTOR_TLM_CPP
