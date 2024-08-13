#ifndef IPS_FILTER_TLM_HPP
#define IPS_FILTER_TLM_HPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;


#include "ips_filter_lt_model.hpp"
#include "../src/img_target.cpp"

#include "important_defines.hpp"

//Extended Unification TLM
struct ips_filter_tlm : public Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE>, public img_target
{
    
    ips_filter_tlm(sc_module_name name, bool use_prints_) : Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE>((std::string(name) + "_HW_block").c_str()), img_target((std::string(name) + "_target").c_str()) {
        set_mem_attributes(IMG_FILTER_KERNEL_ADDRESS_LO, IMG_FILTER_KERNEL_SIZE+IMG_FILTER_OUTPUT_SIZE);
        use_prints = use_prints_;
        checkprintenableimgtar(use_prints);
    }
    
    //Override do_when_transaction functions
    virtual void do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    virtual void do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    
    IPS_IN_TYPE_TB img_window[IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE];
    IPS_OUT_TYPE_TB img_result;
    
};
#endif // IPS_FILTER_TLM_HPP