#ifndef SOBEL_EDGE_DETECTOR_TLM_HPP
#define SOBEL_EDGE_DETECTOR_TLM_HPP
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "sobel_edge_detector_at_model.hpp"
#include "../src/img_target.cpp"
#include "address_map.hpp"

//Extended Unification TLM
struct sobel_edge_detector_tlm : public Edge_Detector, public img_target
{

    SC_CTOR(sobel_edge_detector_tlm): Edge_Detector(Edge_Detector::name()), img_target(img_target::name()) {
        sobel_input = new sc_uint<8>[9];
        set_mem_attributes(SOBEL_INPUT_0_ADDRESS_LO, SOBEL_INPUT_0_SIZE+SOBEL_INPUT_1_SIZE);
    }

    //Override do_when_transaction functions
    virtual void do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);
    virtual void do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address);

    void read() override;
    void write() override;

    sc_uint<8> *sobel_input;
};
#endif // SOBEL_EDGE_DETECTOR_TLM_HPP
