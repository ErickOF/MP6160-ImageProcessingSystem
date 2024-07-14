#ifndef MEMORY_TLM_HPP
#define MEMORY_TLM_HPP
#include "systemc.h"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

#include "../src/img_target.cpp"

//Extended Unification TLM
struct memory_tlm : public img_target
{
    
    SC_CTOR(memory_tlm): img_target(img_target::name()) {
      mem_array = new unsigned char[2764852];
    }
    
    //Override do_when_transaction functions
    virtual void do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    virtual void do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    
    void backdoor_write(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    void backdoor_read(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    
    unsigned char* mem_array;
    
    sc_uint<64> mem_data;
    sc_uint<24> mem_address;
    sc_uint<1>  mem_we;
    
};
#endif // MEMORY_TLM_HPP
