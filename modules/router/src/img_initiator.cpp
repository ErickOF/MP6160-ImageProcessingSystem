

#ifndef IMG_INITIATOR_CPP
#define IMG_INITIATOR_CPP

// #include "tlm_transaction.cpp"
#include "transaction_memory_manager.cpp"

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

//const char* tlm_enum_names[] = {"TLM_ACCEPTED", "TLM_UPDATED", "TLM_COMPLETED"}; 

// Initiator module generating generic payload transactions   
struct img_initiator: sc_module 
{   
  // TLM2.0 Socket  
  tlm_utils::simple_initiator_socket<img_initiator> socket;

  // Memory Manager for transaction memory allocation
  mm memory_manager;

  //Address for this Initiator
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

  bool use_prints;
  
  //Constructor
  SC_CTOR(img_initiator)   
  : socket("socket"), m_peq(this, &img_initiator::peq_cb), use_prints(true) // Construct and name socket   
  {   
    // Register callbacks for incoming interface method calls
    socket.register_nb_transport_bw(this, &img_initiator::nb_transport_bw);
  }

  //Method to send_reading transaction and wait for response
  void read (unsigned char*& data, unsigned int address, unsigned int data_length){

    //Create transaction and allocate it
    tlm::tlm_generic_payload* transaction = memory_manager.allocate();
    transaction->acquire();
    img_generic_extension* img_ext = new img_generic_extension;
    img_ext->transaction_number = this->transaction_number++;
    
    //Set transaction fields
    transaction->set_command(tlm::TLM_READ_COMMAND);
    transaction->set_address(address);
    transaction->set_data_ptr(data);
    transaction->set_data_length(data_length); //In Bytes
    transaction->set_streaming_width(data_length);
    transaction->set_byte_enable_ptr(0);
    transaction->set_dmi_allowed(false); //Mandatory Initial Value
    transaction->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE); //Mandatory Initial Value
    transaction->set_extension(img_ext);

    //Send transaction
    this->send_transaction(transaction);

    data = transaction->get_data_ptr();
    //-----------DEBUG-----------
    dbgmodprint(use_prints, "Reading at Initiator: ");
    for (long unsigned int i = 0; i < transaction->get_data_length()/sizeof(int); ++i){
      dbgmodprint(use_prints, "%02x", *(reinterpret_cast<int*>(transaction->get_data_ptr())+i));
    }
    //-----------DEBUG-----------
  }

  void write (unsigned char*& data, unsigned int address, unsigned int data_length){

    //Create transaction and allocate it
    tlm::tlm_generic_payload* transaction = memory_manager.allocate();
    img_generic_extension* img_ext = new img_generic_extension;
    img_ext->transaction_number = this->transaction_number++;

    //Set transaction fields
    transaction->set_command(tlm::TLM_WRITE_COMMAND);
    transaction->set_address(address);
    transaction->set_data_ptr(data);
    transaction->set_data_length(data_length); //In Bytes
    transaction->set_streaming_width(data_length);
    transaction->set_byte_enable_ptr(0);
    transaction->set_dmi_allowed(false); //Mandatory Initial Value
    transaction->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE); //Mandatory Initial Value
    transaction->set_extension(img_ext);

    //-----------DEBUG-----------
    dbgmodprint(use_prints, "Writing: ");
    for (long unsigned int i = 0; i < data_length/sizeof(int); ++i){
      dbgmodprint(use_prints, "%02x", *(reinterpret_cast<int*>(transaction->get_data_ptr())+i));
    }
    //-----------DEBUG-----------

    //Set transaction
    this->send_transaction(transaction);
  }
  
  void send_transaction(tlm::tlm_generic_payload*& transaction) {

    //Transaction Management Variables
    tlm::tlm_phase phase;
    tlm::tlm_sync_enum status;
    tlm::tlm_command cur_command;
    img_generic_extension* img_ext;

    //Begin Request
    phase = tlm::BEGIN_REQ;
    transaction->get_extension(img_ext);
    cur_command = transaction->get_command();

    this->transaction_sent_id = img_ext->transaction_number;

    dbgmodprint(use_prints, "BEGIN_REQ SENT TRANS ID %0d", img_ext->transaction_number);
    status = socket->nb_transport_fw(*transaction, phase, ((cur_command == tlm::TLM_WRITE_COMMAND) ? this->write_delay : this->read_delay));  // Non-blocking transport call   

    // Check request status returned by target
    switch (status) {   
        //Case 1: Transaction was accepted
        case tlm::TLM_ACCEPTED: {
          dbgmodprint(use_prints, "%s received -> Transaction ID %d", "TLM_ACCEPTED", img_ext->transaction_number);
          check_transaction(*transaction);
          //transaction->release();
          //Initiator only cares about sending the transaction, doesnt need to wait for response (non-blocking)
          break;
        }

        //Not implementing Updated and Completed Status
        default: {
          dbgmodprint(use_prints, "[ERROR] Invalid status received at initiator -> Transaction ID %d", img_ext->transaction_number);
          break;
        }
    }

    //Wait for response transaction
    // if (transaction->get_command() == tlm::TLM_READ_COMMAND) {
    wait(transaction_received_e);
    this->transaction_received_id = img_ext->transaction_number;
    // }
    //-----------DEBUG-----------
    dbgmodprint(use_prints, "[DEBUG1] Reading at Initiator: ");
    for (long unsigned int i = 0; i < transaction->get_data_length()/sizeof(int); ++i){
      dbgmodprint(use_prints, "%02x", *(reinterpret_cast<int*>(transaction->get_data_ptr())+i));
    }
    //-----------DEBUG-----------

    //Increment transaction ID
  }   
 
   
  // TLM2 backward path non-blocking transport method
  virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& trans,   
                                             tlm::tlm_phase& phase, sc_time& delay )   
  { 
    //Call event queue
    m_peq.notify(trans, phase, delay);
    return tlm::TLM_ACCEPTED;
  }

  //Payload event and queue callback to handle transactions received from target
  void peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase)
  {

    //dbgmodprint(use_prints, "%s received -> Transaction ID %d from address %x", phase, this->id_extension->transaction_id);
    //cout << name() << " " <<hex << trans.get_address() << " BEGIN_RESP RECEIVED at " << sc_time_stamp() << endl;
    switch (phase) {
      case tlm::BEGIN_RESP: {

        check_transaction(trans);

        //Initiator dont care about confirming resp transaction. So nothing else to do.

        //-----------DEBUG-----------
        dbgmodprint(use_prints, "[DEBUG] Reading at Initiator: ");
        for (long unsigned int i = 0; i < trans.get_data_length()/sizeof(int); ++i){
          dbgmodprint(use_prints, "%02x", *(reinterpret_cast<int*>(trans.get_data_ptr())+i));
        }
        //-----------DEBUG-----------

        transaction_received_e.notify();
        //-----------DEBUG-----------
        dbgmodprint(use_prints, "[DEBUG] Reading at Initiator: ");
        for (long unsigned int i = 0; i < trans.get_data_length()/sizeof(int); ++i){
          dbgmodprint(use_prints, "%02x", *(reinterpret_cast<int*>(trans.get_data_ptr())+i));
        }
        //-----------DEBUG-----------
        break;
      }
      default: {
        SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by initiator");
        break;
      }
    }
  }

  //Function to check transaction integrity
  void check_transaction(tlm::tlm_generic_payload& trans)
  {
    //Check transaction if here

    // tlm::tlm_command command  = trans.get_command();
    // sc_dt::uint64    address  = trans.get_address();
    // unsigned char*   data_ptr = reinterpret_cast<unsigned char*>(trans.get_data_ptr());

    // Allow the memory manager to free the transaction object
    //trans.release();
  }
  
  void start_img_initiators()
  {
    transaction_number = 0;
  }
  
  void set_delays(sc_time w_delay, sc_time r_delay)
  {
    this->write_delay = w_delay;
    this->read_delay = r_delay;
  }
} ; 

#endif // IMG_INITIATOR_CPP
