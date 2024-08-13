#ifndef RGB2GRAY_TLM_HPP
#define RGB2GRAY_TLM_HPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#include "rgb2gray_pv_model.hpp"
#include "../src/img_target.cpp"

//Extended Unification TLM
struct rgb2gray_tlm : public Rgb2Gray, public img_target
{

    rgb2gray_tlm(sc_module_name name) : Rgb2Gray((std::string(name) + "_HW_block").c_str()), img_target((std::string(name) + "_target").c_str()) {
#ifdef DISABLE_RGB_DEBUG
        this->use_prints = false;
#endif //DISABLE_RGB_DEBUG
        checkprintenableimgtar(use_prints);
    }

    //Override do_when_transaction functions
    virtual void do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    virtual void do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);

};
#endif // RGB2GRAY_TLM_HPP
