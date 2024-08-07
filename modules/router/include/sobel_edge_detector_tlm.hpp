#ifndef SOBEL_EDGE_DETECTOR_TLM_HPP
#define SOBEL_EDGE_DETECTOR_TLM_HPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#include "sobel_edge_detector_at_model.hpp"
#include "../src/img_target.cpp"

//Extended Unification TLM
struct sobel_edge_detector_tlm : public Edge_Detector, public img_target
{

    sobel_edge_detector_tlm(sc_module_name name) : Edge_Detector((std::string(name) + "_HW_block").c_str()), img_target((std::string(name) + "_target").c_str()) {
    }

    //Override do_when_transaction functions
    virtual void do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    virtual void do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);

    void read() override;
    void write() override;
};
#endif // SOBEL_EDGE_DETECTOR_TLM_HPP
