#ifndef IMG_RECEIVER_TLM_HPP
#define IMG_RECEIVER_TLM_HPP

#include <systemc.h>
#include "img_receiver.hpp"
#include "../src/img_target.cpp"
#include "address_map.hpp"

struct img_receiver_tlm: public img_receiver, public img_target
{
    img_receiver_tlm(sc_module_name name) : img_receiver((std::string(name) + "_receiver").c_str()), img_target((std::string(name) + "_target").c_str()) {
        set_mem_attributes(IMG_INPUT_ADDRESS_LO, IMG_INPUT_SIZE);
    }

    //Override do_when_transaction functions
    virtual void do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    virtual void do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);

};

#endif // IMG_RECEIVER_TLM_HPP

