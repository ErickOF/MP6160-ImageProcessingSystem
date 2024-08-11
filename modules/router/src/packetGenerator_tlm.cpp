#ifndef PACKET_GENERATOR_TLM_CPP
#define PACKET_GENERATOR_TLM_CPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#include "packetGenerator_tlm.hpp"

#include "common_func.hpp"

#include "address_map.hpp"

void packetGenerator_tlm::do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address){
  dbgimgtarmodprint(use_prints, "Calling do_when_read_transaction");

  if ((address >= IMG_OUTPUT_STATUS_ADDRESS_LO) && (address < IMG_OUTPUT_STATUS_ADDRESS_HI))
  {
    if (packetGenerator::tmp_data_out_valid == true)
    {
      *data = 1;
    }
    else
    {
      *data = 0;
    }
  }
}

void packetGenerator_tlm::do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address)
{
  dbgimgtarmodprint(use_prints, "Calling do_when_write_transaction");

  if ((address >= IMG_OUTPUT_ADDRESS_LO) && (address < IMG_OUTPUT_SIZE_ADDRESS_LO))
  {
    memcpy(tmp_data + address - IMG_OUTPUT_ADDRESS_LO, data, data_length);
  }
  else if ((address >= IMG_OUTPUT_SIZE_ADDRESS_LO) && (address < IMG_OUTPUT_DONE_ADDRESS_LO))
  {
    unsigned char *data_length_ptr = (unsigned char *)&tmp_data_length;
    memcpy(data_length_ptr + address - IMG_OUTPUT_SIZE_ADDRESS_LO, data, data_length);

    dbgimgtarmodprint(use_prints, "Current data_length %0d", tmp_data_length);
  }
  else if ((address >= IMG_OUTPUT_DONE_ADDRESS_LO) && (address < IMG_OUTPUT_STATUS_ADDRESS_LO) && (*data == 1))
  {
    if (tmp_data_length == 0)
    {
      *(tmp_data) = 0;
      tmp_data_length = 1;
    }

    dbgimgtarmodprint(use_prints, "Preparing to send %0d bytes", tmp_data_length);

    fill_data(tmp_data, (int)tmp_data_length);

    tmp_data_length = 0;
  }
}

#endif // PACKET_GENERATOR_TLM_CPP
