#ifndef IPS_FILTER_TLM_CPP
#define IPS_FILTER_TLM_CPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#include "ips_filter_tlm.hpp"

#include "common_func.hpp"
#include "important_defines.hpp"

void ips_filter_tlm::do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address)
{
  this->img_result = *(Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE>::result_ptr);
  *data = (unsigned char) this->img_result;
  //memcpy(data, Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE>::result_ptr, sizeof(IPS_OUT_TYPE_TB));
}

void ips_filter_tlm::do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address)
{
  IPS_OUT_TYPE_TB* result = new IPS_OUT_TYPE_TB;
  IPS_IN_TYPE_TB* img_window = new IPS_IN_TYPE_TB[3 * 3];

  //dbgprint("[DEBUG]: data: %0d, address %0d, data_length %0d, size of char %0d", *data, address, data_length, sizeof(char));
  this->img_window[address] = (IPS_IN_TYPE_TB) *data;
  //dbgprint("[DEBUG]: img_window data: %0f", this->img_window[address]);

  if (address == 8) {
    filter(this->img_window, result);
  }
}

#endif // IPS_FILTER_TLM_CPP
