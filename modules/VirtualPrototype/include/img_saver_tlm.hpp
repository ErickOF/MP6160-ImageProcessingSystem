#ifndef IMG_SAVER_TLM_HPP
#define IMG_SAVER_TLM_HPP

#include <systemc.h>
#include "../src/img_target.cpp"
#include "address_map.hpp"

struct img_saver_tlm: public img_target
{
    img_saver_tlm(sc_module_name name) : img_target((std::string(name) + "_target").c_str()) {
        set_mem_attributes(IMG_SAVER_ID_ADDRESS_LO, IMG_SAVER_ID_SIZE+IMG_SAVER_START_SIZE);
    }

    //Override do_when_transaction functions
    virtual void do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    virtual void do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);

    int local_id;

    unsigned char *img_input_ptr;
    unsigned char *img_inprocess_a_ptr;
    unsigned char *img_inprocess_b_ptr;
    unsigned char *img_inprocess_c_ptr;
    unsigned char *img_inprocess_d_ptr;
    unsigned char *img_output_ptr;
    unsigned char *img_output_dec_ptr;
};

#endif // IMG_SAVER_TLM_HPP

