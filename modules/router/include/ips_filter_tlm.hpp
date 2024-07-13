#ifndef IPS_FILTER_TLM_HPP
#define IPS_FILTER_TLM_HPP
#include "systemc.h"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

#include "ips_filter_lt_model.hpp"
#include "../src/img_target.cpp"

#include "ImportantDefines.h"

//Extended Unification TLM
struct ips_filter_tlm : public Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE>, public img_target
{
    
    SC_CTOR(ips_filter_tlm): Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE>(Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE>::name()), img_target(img_target::name()) {
    }
    
    //Override do_when_transaction functions
    virtual void do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    virtual void do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    
    IPS_IN_TYPE_TB img_window[IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE];
    IPS_OUT_TYPE_TB img_result;
    
};
#endif // IPS_FILTER_TLM_HPP
