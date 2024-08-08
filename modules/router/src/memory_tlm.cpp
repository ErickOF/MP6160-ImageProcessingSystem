#ifndef MEMORY_TLM_CPP
#define MEMORY_TLM_CPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#include "memory_tlm.hpp"

#include "common_func.hpp"

#include "address_map.hpp"

void memory_tlm::do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address)
{
  sc_uint<8> local_data[8];
  int max_data_length = (data_length > 8) ? 8 : data_length;
  
  for (int i = 0; i < max_data_length; i++)
  {
    local_data[i] = *(mem_array + address - MEMORY_ADDRESS_LO + i);
  }
  for (int i = max_data_length; i < 8; i++)
  {
    local_data[i] = 0;
  }
  mem_data = (local_data[7], local_data[6], local_data[5], local_data[4], local_data[3], local_data[2], local_data[1], local_data[0]);
  mem_address = address - MEMORY_ADDRESS_LO;
  mem_we = 0;
  memcpy(data, (mem_array + address - MEMORY_ADDRESS_LO), data_length);
}

void memory_tlm::do_when_write_transaction(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  sc_uint<8> local_data[8];
  int max_data_length = (data_length > 8) ? 8 : data_length;
  
  for (int i = 0; i < max_data_length; i++)
  {
    local_data[i] = *(data + i);
  }
  for (int i = max_data_length; i < 8; i++)
  {
    local_data[i] = 0;
  }
  mem_data = (local_data[7], local_data[6], local_data[5], local_data[4], local_data[3], local_data[2], local_data[1], local_data[0]);
  mem_address = address - MEMORY_ADDRESS_LO;
  mem_we = 1;
  memcpy((mem_array + address - MEMORY_ADDRESS_LO), data, data_length);
}

void memory_tlm::backdoor_write(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  memcpy((mem_array + address - MEMORY_ADDRESS_LO), data, data_length);
}

void memory_tlm::backdoor_read(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  data = new unsigned char[data_length];
  memcpy(data, (mem_array + address - MEMORY_ADDRESS_LO), data_length);
}

#endif // MEMORY_TLM_CPP
