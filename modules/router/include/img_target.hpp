#ifndef IMG_TARGET_HPP
#define IMG_TARGET_HPP

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#include "common_func.hpp"
#include "img_generic_extension.hpp"

// For an internal response phase
DECLARE_EXTENDED_PHASE(internal_processing_ph);


// Initiator module generating generic payload transactions   
struct img_target: sc_module 
{
    // TLM2.0 Socket  
    tlm_utils::simple_target_socket<img_target> socket;

    // Pointer to transaction in progress
    tlm::tlm_generic_payload* response_transaction; 

    // Payload event queue with callback and phase
    tlm_utils::peq_with_cb_and_phase<img_target> m_peq;

    // Delay
    sc_time response_delay;
    sc_time receive_delay;

    // SC_EVENT
    sc_event send_response_e;

    // DEBUG
    unsigned int transaction_in_progress_id = 0;

    // Construct and name socket
    SC_CTOR(img_target)   
    : socket("socket"), response_transaction(0), m_peq(this, &img_target::peq_cb)
    {   
        // Register callbacks for incoming interface method calls
        socket.register_nb_transport_fw(this, &img_target::nb_transport_fw);

        SC_THREAD(send_response);
    }

    tlm::tlm_sync_enum nb_transport_fw(
        tlm::tlm_generic_payload& trans,
        tlm::tlm_phase& phase, sc_time& delay
    );

    // Payload and Queue Callback
    void peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase);

    // Response function
    void send_response();

    virtual void do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address) {}

    virtual void do_when_write_transaction(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address) {}

    //Thread to call nb_transport on the backward path -> here the module process data and responds to initiator
    void process_transaction(tlm::tlm_generic_payload& trans);
    
    void set_delays(sc_time resp_delay, sc_time rec_delay);
};
#endif // IMG_TARGET_HPP
