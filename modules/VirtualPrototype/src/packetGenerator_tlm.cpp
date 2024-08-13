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
  dbgimgtarmodprint(use_prints, "Called do_when_read_transaction with an address %016llX and length %d", address, data_length);

  if ((address >= IMG_OUTPUT_SIZE + IMG_OUTPUT_SIZE_SIZE + IMG_OUTPUT_DONE_SIZE) && (address < IMG_OUTPUT_SIZE + IMG_OUTPUT_SIZE_SIZE + IMG_OUTPUT_DONE_SIZE + IMG_OUTPUT_STATUS_SIZE))
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
  else
  {
    *data = 0;
  }
}

void packetGenerator_tlm::do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address)
{
  dbgimgtarmodprint(use_prints, "Called do_when_write_transaction with an address %016llX and length %d", address, data_length);

  if (address < IMG_OUTPUT_SIZE)
  {
    memcpy(tmp_data + address, data, data_length);
  }
  else if ((address >= IMG_OUTPUT_SIZE) && (address < IMG_OUTPUT_SIZE + IMG_OUTPUT_SIZE_SIZE))
  {
    unsigned char *data_length_ptr = (unsigned char *)&tmp_data_length;
    memcpy(data_length_ptr + address - IMG_OUTPUT_SIZE, data, data_length);

    dbgimgtarmodprint(use_prints, "Current data_length %0d", tmp_data_length);
  }
  else if ((address >= IMG_OUTPUT_SIZE + IMG_OUTPUT_SIZE_SIZE) && (address < IMG_OUTPUT_SIZE + IMG_OUTPUT_SIZE_SIZE + IMG_OUTPUT_DONE_SIZE) && (*data == 1))
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

//Backdoor write/read for debug purposes
void packetGenerator_tlm::backdoor_write(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  sc_dt::uint64 local_address = address - IMG_OUTPUT_ADDRESS_LO;
  memcpy((tmp_data + local_address), data, data_length);
  for (int i = 0; (i < 10) && (local_address + i < IMG_OUTPUT_SIZE); i++) {
  	dbgimgtarmodprint(use_prints, "Backdoor Writing: %0d\n", *(tmp_data + local_address + i));
  }
}

void packetGenerator_tlm::backdoor_read(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  sc_dt::uint64 local_address = address - IMG_OUTPUT_ADDRESS_LO;
  data = new unsigned char[data_length];
  memcpy(data, (tmp_data + local_address), data_length);
  for (int i = 0; (i < 10) && (local_address + i < IMG_OUTPUT_SIZE); i++) {
  	dbgimgtarmodprint(use_prints, "Backdoor Reading: %0d\n", *(tmp_data + local_address + i));
  }
}

#endif // PACKET_GENERATOR_TLM_CPP
