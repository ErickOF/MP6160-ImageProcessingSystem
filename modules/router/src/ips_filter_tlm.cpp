#ifndef IPS_FILTER_TLM_CPP
#define IPS_FILTER_TLM_CPP
#include "systemc.h"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

#include "ips_filter_tlm.hpp"

#include "common_func.hpp"
#include "ImportantDefines.h"

void ips_filter_tlm::do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address)
{
  this->img_result = *(Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE>::result_ptr);
  memcpy(data, Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE>::result_ptr, sizeof(IPS_OUT_TYPE_TB));
}

void ips_filter_tlm::do_when_write_transaction(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  IPS_OUT_TYPE_TB* result = new IPS_OUT_TYPE_TB;
  IPS_IN_TYPE_TB* img_window = new IPS_IN_TYPE_TB[3 * 3];
  
  if ((IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE * sizeof(char)) != data_length)
  {
    SC_REPORT_FATAL("IPS_FILTER", "Illegal transaction size");
  }
  
  for (int i = 0; i < IPS_FILTER_KERNEL_SIZE; i++)
  {
    for (int j = 0; j < IPS_FILTER_KERNEL_SIZE; j++)
    {
      *(img_window + ((i * IPS_FILTER_KERNEL_SIZE) + j)) = (IPS_IN_TYPE_TB)*(data + ((i * IPS_FILTER_KERNEL_SIZE) + j));
      this->img_window[(i * IPS_FILTER_KERNEL_SIZE) + j] = (IPS_IN_TYPE_TB)*(data + ((i * IPS_FILTER_KERNEL_SIZE) + j));
    }
  }
  
  filter(img_window, result);
}

#endif // IPS_FILTER_TLM_CPP
