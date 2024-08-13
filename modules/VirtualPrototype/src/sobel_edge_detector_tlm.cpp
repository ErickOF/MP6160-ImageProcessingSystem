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
  unsigned char sobel_results;
  unsigned char* sobel_results_ptr;
  sc_uint<8> local_result;

  dbgimgtarmodprint(use_prints, "Called do_when_read_transaction with an address %016llX and length %d", address, data_length);

  if ((address >= (SOBEL_INPUT_0_SIZE + SOBEL_INPUT_1_SIZE)) && (address < (SOBEL_INPUT_0_SIZE + SOBEL_INPUT_1_SIZE + SOBEL_OUTPUT_SIZE)))
  {
    Edge_Detector::address = address + SOBEL_INPUT_0_ADDRESS_LO;
    read();
    
    for (int i = 0; i < data_length; i++)
    {
      local_result = Edge_Detector::data.range((i + 1) * 8 - 1, i * 8);
      sobel_results = (unsigned char)local_result;
      sobel_results_ptr = &sobel_results;
      memcpy((data + i), sobel_results_ptr, sizeof(char));
      // dbgimgtarmodprint(use_prints, "Results on sobel read %01X", sobel_results);
    }
  }
  else
  {
    sobel_results = 0;
    sobel_results_ptr = &sobel_results;

    for (int i =0; i < data_length; i++)
    {
      memcpy((data + i), sobel_results_ptr, sizeof(char));
      // dbgimgtarmodprint(use_prints, "Results on sobel read %01X", sobel_results);
    }
  }
  
}

void sobel_edge_detector_tlm::do_when_write_transaction(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  dbgimgtarmodprint(use_prints, "Called do_when_write_transaction with an address %016llX and length %d", address, data_length);

  if (address < (SOBEL_INPUT_0_SIZE + SOBEL_INPUT_1_SIZE))
  {
    if (address < SOBEL_INPUT_0_SIZE) {
      for (int i = 0; i < data_length; i++) {
        this->sobel_input[address + i] = *(data + i);
      }
    }
    else if (SOBEL_INPUT_0_SIZE <= address && address < SOBEL_INPUT_0_SIZE + SOBEL_INPUT_1_SIZE) {
      for (int i = 0; i < data_length; i++) {
        this->sobel_input[address + i - SOBEL_INPUT_0_SIZE] = *(data + i);
      }
    }

    Edge_Detector::data = (this->sobel_input[7], this->sobel_input[6], this->sobel_input[5], this->sobel_input[4], this->sobel_input[3], this->sobel_input[2], this->sobel_input[1], this->sobel_input[0]);
    Edge_Detector::address = address + SOBEL_INPUT_0_ADDRESS_LO;

    write();
  }
  
}

void sobel_edge_detector_tlm::read()
{
  Edge_Detector::data = (sc_uint<32>(0), resultSobelGradientY, resultSobelGradientX);
  Edge_Detector::data = Edge_Detector::data >> ((Edge_Detector::address - SOBEL_OUTPUT_ADDRESS_LO) * 8);
}

void sobel_edge_detector_tlm::write()
{
  wr_t.notify(0, SC_NS);
}

void sobel_edge_detector_tlm::wr()
{
  while(1)
  {
    Edge_Detector::wait(wr_t);
    if ((Edge_Detector::address >= SOBEL_INPUT_0_ADDRESS_LO) && (Edge_Detector::address < SOBEL_INPUT_0_ADDRESS_HI))
    {
      int j = 0;
      localWindow[0][0] = Edge_Detector::data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[0][1] = Edge_Detector::data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[0][2] = Edge_Detector::data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[1][0] = Edge_Detector::data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[1][1] = Edge_Detector::data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[1][2] = Edge_Detector::data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[2][0] = Edge_Detector::data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[2][1] = Edge_Detector::data.range((j + 1) * 8 - 1, j * 8);
      gotLocalWindow.notify(0, SC_NS);
    }
    else if ((Edge_Detector::address >= SOBEL_INPUT_1_ADDRESS_LO) && (Edge_Detector::address < SOBEL_INPUT_1_ADDRESS_HI))
    {
      localWindow[2][2] = data.range(7, 0);
      gotLocalWindow.notify(0, SC_NS);
    }
  }
}

#endif // SOBEL_EDGE_DETECTOR_TLM_CPP
