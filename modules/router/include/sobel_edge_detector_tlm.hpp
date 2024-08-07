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
#include "img_target.hpp"

//Extended Unification TLM
struct sobel_edge_detector_tlm : public Edge_Detector, public img_target
{

    SC_CTOR(sobel_edge_detector_tlm): Edge_Detector(Edge_Detector::name()), img_target(img_target::name()) {
    }

    //Override do_when_transaction functions
    virtual void do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    virtual void do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);

    void read() override;
    void write() override;
};
#endif // SOBEL_EDGE_DETECTOR_TLM_HPP
