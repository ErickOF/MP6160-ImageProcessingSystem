#ifndef MEMORY_TLM_HPP
#define MEMORY_TLM_HPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#include "../src/img_target.cpp"
#include "address_map.hpp"

//Extended Unification TLM
struct memory_tlm : public img_target
{
    
    memory_tlm(sc_module_name name) : img_target((std::string(name) + "_target").c_str()) {
      mem_array = new unsigned char[MEMORY_SIZE];
#ifdef DISABLE_MEM_DEBUG
      this->use_prints = false;
#endif //DISABLE_MEM_DEBUG
      checkprintenableimgtar(use_prints);
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
