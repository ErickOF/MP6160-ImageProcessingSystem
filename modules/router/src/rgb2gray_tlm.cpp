#ifndef RGB2GRAY_TLM_CPP
#define RGB2GRAY_TLM_CPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#include "rgb2gray_tlm.hpp"

#include "common_func.hpp"

void rgb2gray_tlm::do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address){
  unsigned char pixel_value;
  
  pixel_value = this->obtain_gray_value();
  dbgimgtarmodprint("%0u", pixel_value);
  memcpy(data, &pixel_value, sizeof(char));
}

void rgb2gray_tlm::do_when_write_transaction(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address){
  unsigned char rgb_values[3];
  int j = 0;

  for (unsigned char *i = data; (i - data) < 3; i++)
  {
    rgb_values[j] = *i;
    dbgimgtarmodprint("VAL: %0ld -> %0u", i - data, *i);
    j++;
  }

  this->set_rgb_pixel(rgb_values[0], rgb_values[1], rgb_values[2]);
}

#endif // RGB2GRAY_TLM_CPP
