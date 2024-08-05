#ifndef IMG_TRANSMITER_TLM_HPP
#define IMG_TRANSMITER_TLM_HPP

#include <systemc.h>
#include "img_transmiter.hpp"
#include "../src/img_target.cpp"
#include "address_map.hpp"

struct img_transmiter_tlm: public img_transmiter, public img_target
{
    SC_CTOR(img_transmiter_tlm): img_transmiter(img_transmiter::name()), img_target(img_target::name()) {
        set_mem_attributes(IMG_OUTPUT_ADDRESS_LO, IMG_OUTPUT_SIZE);
    }

    //Override do_when_transaction functions
    virtual void do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    virtual void do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);

};

#endif // IMG_TRANSMITER_TLM_HPP

