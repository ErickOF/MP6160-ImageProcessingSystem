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
  dbgimgtarmodprint(use_prints, "Called do_when_read_transaction with an address %016llX and length %d", address, data_length);

  this->img_result = *(Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE>::result_ptr);
  *data = (unsigned char) this->img_result;
  //memcpy(data, Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE>::result_ptr, sizeof(IPS_OUT_TYPE_TB));
}

void ips_filter_tlm::do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address)
{
  IPS_OUT_TYPE_TB* result = new IPS_OUT_TYPE_TB;

  dbgimgtarmodprint(use_prints, "Called do_when_write_transaction with an address %016llX and length %d", address, data_length);

  //dbgimgtarmodprint(use_prints, "[DEBUG]: data: %0d, address %0d, data_length %0d, size of char %0d", *data, address, data_length, sizeof(char));
  for (int i = 0; i < data_length; i++) {
    this->img_window[address + i] = (IPS_IN_TYPE_TB) *(data + i);
  }
  //dbgimgtarmodprint(use_prints, "[DEBUG]: img_window data: %0f", this->img_window[address]);

  if (address == 8) {
    filter(this->img_window, result);
  }
}

#endif // IPS_FILTER_TLM_CPP
