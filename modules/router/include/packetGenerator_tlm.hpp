#ifndef PACKET_GENERATOR_TLM_HPP
#define PACKET_GENERATOR_TLM_HPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#include <systemc-ams.h>

#include "packetGenerator.h"
#include "../src/img_target.cpp"
#include "address_map.hpp"

//Extended Unification TLM
struct packetGenerator_tlm : public packetGenerator, public img_target
{

    packetGenerator_tlm(sc_module_name name, sca_core::sca_time sample_time) : packetGenerator((std::string(name) + "_AMS_HW_block").c_str(), sample_time), img_target((std::string(name) + "_target").c_str()), tmp_data_length(0) {
       tmp_data = new unsigned char[IMG_OUTPUT_SIZE];
#ifdef DISABLE_PACKET_GENERATOR_DEBUG
        this->use_prints = false;
#endif // DISABLE_PACKET_GENERATOR_DEBUG
        checkprintenableimgtar(use_prints);
    }

    //Override do_when_transaction functions
    virtual void do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    virtual void do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);

    unsigned int tmp_data_length;
    unsigned char* tmp_data;

};
#endif // PACKET_GENERATOR_TLM_HPP
