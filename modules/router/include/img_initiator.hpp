

#ifndef IMG_INITIATOR_HPP
#define IMG_INITIATOR_HPP

#include "transaction_memory_manager.hpp"

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

// const char* tlm_enum_names[] = {"TLM_ACCEPTED", "TLM_UPDATED", "TLM_COMPLETED"};

// Initiator module generating generic payload transactions
struct img_initiator : sc_module
{
  // TLM2.0 Socket
  tlm_utils::simple_initiator_socket<img_initiator> socket;

  // Memory Manager for transaction memory allocation
  mm memory_manager;

  // Address for this Initiator
  unsigned int data_length;
  unsigned int transaction_number;

  // Payload event queue with callback and phase
  tlm_utils::peq_with_cb_and_phase<img_initiator> m_peq;

  // Events
  sc_event transaction_received_e;

  // Delay
  sc_time write_delay;
  sc_time read_delay;

  // DEBUG
  unsigned int transaction_sent_id = 0;
  unsigned int transaction_received_id = 0;

  // Construct and name socket
  SC_CTOR(img_initiator)
      : socket("socket"), m_peq(this, &img_initiator::peq_cb)
  {
    // Register callbacks for incoming interface method calls
    socket.register_nb_transport_bw(this, &img_initiator::nb_transport_bw);
  }

  // Method to send_reading transaction and wait for response
  void read(unsigned char *&data, unsigned int address, unsigned int data_length);
  void write(unsigned char *&data, unsigned int address, unsigned int data_length);
  void send_transaction(tlm::tlm_generic_payload *&transaction);

  // TLM2 backward path non-blocking transport method
  virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &trans,
                                             tlm::tlm_phase &phase, sc_time &delay);

  // Payload event and queue callback to handle transactions received from target
  void peq_cb(tlm::tlm_generic_payload &trans, const tlm::tlm_phase &phase);

  // Function to check transaction integrity
  void check_transaction(tlm::tlm_generic_payload &trans);
  void start_img_initiators();
  void set_delays(sc_time w_delay, sc_time r_delay);
};
#endif // IMG_INITIATOR_HPP
