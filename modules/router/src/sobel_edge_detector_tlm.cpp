#ifndef SOBEL_EDGE_DETECTOR_TLM_CPP
#define SOBEL_EDGE_DETECTOR_TLM_CPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#include "sobel_edge_detector_tlm.hpp"

#include "common_func.hpp"

void sobel_edge_detector_tlm::do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address){
  short int sobel_results[4];
  sc_int<16> local_result;
  
  dbgimgtarmodprint(use_prints, "Calling do_when_read_transaction");

  Edge_Detector::address = address;
  read();
  
  for (int i = 0; i < 4; i++)
  {
    local_result = Edge_Detector::data.range((i + 1) * 16 - 1, i * 16).to_int();
    sobel_results[i] = (short int)local_result;
    dbgimgtarmodprint(use_prints, "%0d", sobel_results[i]);
  }
  
  memcpy(data, sobel_results, 4 * sizeof(short int));
}

void sobel_edge_detector_tlm::do_when_write_transaction(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  sc_uint<8> values[8];
  
  dbgimgtarmodprint(use_prints, "Calling do_when_write_transaction");
  
  for (int i = 0; i < 8; i++)
  {
    values[i] = *(data + i);
  }
  Edge_Detector::data = (values[7], values[6], values[5], values[4], values[3], values[2], values[1], values[0]);
  Edge_Detector::address = address;
  write();
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
