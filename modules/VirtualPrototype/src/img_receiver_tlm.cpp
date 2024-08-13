#ifndef IMG_RECEIVER_TLM_CPP
#define IMG_RECEIVER_TLM_CPP

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include "img_receiver_tlm.hpp"
#include "address_map.hpp"

#include "common_func.hpp"

void img_receiver_tlm::do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address)
{
    for (int i = 0; i < data_length; i++){
        *(data+i) = *(input_image+address+i);
    }
}

void img_receiver_tlm::do_when_write_transaction(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  for (int i = 0; i < data_length; i++){
    *(input_image+address+i) = *(data+i);
  }
}

#endif // IMG_RECEIVER_TLM_CPP
