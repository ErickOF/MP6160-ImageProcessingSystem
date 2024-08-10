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

  if ((address = ETHERNET_CHECK_DONE) && (data_length == 1))
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

  if ((address >= ETHERNET_DATA_WR) && (address + data_length - 1 < ETHERNET_DATA_DONE))
  {

    if (tmp_data_length == 0)
    {
      tmp_data = new unsigned char[data_length];
      memcpy(tmp_data, data, sizeof(char) * data_length);
    }
    else
    {
      unsigned char* tmp_data_;
      tmp_data_ = tmp_data;
      tmp_data = new unsigned char[tmp_data_length + data_length];
      memcpy(tmp_data, tmp_data_, sizeof(char) * tmp_data_length);
      memcpy(tmp_data + tmp_data_length, data, data_length);

      delete[] tmp_data_;
    }

    dbgimgtarmodprint(use_prints, "Previous data_length %0d new data_length %0d", tmp_data_length, tmp_data_length + data_length);
    tmp_data_length = tmp_data_length + data_length;
  }
  else if ((address == ETHERNET_DATA_DONE) && (data_length == 1) && (*data == 1))
  {
    if (tmp_data_length == 0)
    {
      tmp_data = new unsigned char[1];
      *tmp_data = 0;
      tmp_data_length = 1;
    }

    dbgimgtarmodprint(use_prints, "Preparing to send %0d bytes", tmp_data_length);

    fill_data(tmp_data, (int)tmp_data_length);

    delete[] tmp_data;
    tmp_data_length = 0;
  }
}

#endif // PACKET_GENERATOR_TLM_CPP
