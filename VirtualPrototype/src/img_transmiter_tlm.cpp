#ifndef IMG_TRANSMITER_TLM_CPP
#define IMG_TRANSMITER_TLM_CPP

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include "img_transmiter_tlm.hpp"
#include "address_map.hpp"

#include "common_func.hpp"

void img_transmiter_tlm::do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address)
{
  for (int i = 0; i < data_length; i++){
      *(data+i) = *(output_image+address+i);
  }
}

void img_transmiter_tlm::do_when_write_transaction(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  for (int i = 0; i < data_length; i++){
    *(output_image+address+i) = *(data+i);
  }
}

#endif // IMG_TRANSMITER_TLM_CPP
