

#ifndef IMG_INITIATOR_CPP
#define IMG_INITIATOR_CPP

// #include "tlm_transaction.cpp"
#include "transaction_memory_manager.cpp"

#include "systemc.h"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

//const char* tlm_enum_names[] = {"TLM_ACCEPTED", "TLM_UPDATED", "TLM_COMPLETED"}; 

// Initiator module generating generic payload transactions   
struct img_initiator: sc_module 
{   
  // TLM2.0 Socket  
  tlm_utils::simple_initiator_socket<img_initiator> socket;

  //Memory Manager for transaction memory allocation
  mm memory_manager;

  //Address for this Initiator
  unsigned int address;
  unsigned char* data;
  unsigned int data_length; 

  //Pointer to transaction in progress
  tlm::tlm_generic_payload* pending_transaction; 

  //Payload event queue with callback and phase
  tlm_utils::peq_with_cb_and_phase<img_initiator> m_peq;

  //Events
  sc_event transaction_received_e;

  //Delay
  sc_time write_delay = sc_time(10, SC_NS);
  sc_time read_delay = sc_time(10,SC_NS);
  
  //Constructor
  SC_CTOR(img_initiator)   
  : socket("socket"), pending_transaction(0), m_peq(this, &img_initiator::peq_cb) // Construct and name socket   
  {   
    // Register callbacks for incoming interface method calls
    socket.register_nb_transport_bw(this, &img_initiator::nb_transport_bw);
  }

  //Method to send_reading transaction and wait for response
  void read (int*& data, unsigned int address, unsigned int data_length){

    //Create transaction and allocate it
    tlm::tlm_generic_payload* transaction = memory_manager.allocate();
    
    //Set transaction fields
    transaction->set_command(tlm::TLM_READ_COMMAND);
    transaction->set_address(address);
    transaction->set_data_ptr(reinterpret_cast<unsigned char*>(data));
    transaction->set_data_length(data_length); //In Bytes
    transaction->set_streaming_width(data_length);
    transaction->set_byte_enable_ptr(0);
    transaction->set_dmi_allowed(false); //Mandatory Initial Value
    transaction->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE); //Mandatory Initial Value

    //Send transaction
    this->send_transaction(transaction);

    data = reinterpret_cast<int*>(this->data);
    //-----------DEBUG-----------
    printf("[DEBUG] Reading at Initiator: ");
    for (int i = 0; i < this->pending_transaction->get_data_length()/sizeof(int); ++i){
      printf("%02x", *(reinterpret_cast<int*>(this->data)+i));
    }
    printf("\n");
    //-----------DEBUG-----------
  }

  void write (int*& data, unsigned int address, unsigned int data_length){

    //Create transaction and allocate it
    tlm::tlm_generic_payload* transaction = memory_manager.allocate();

    //Set transaction fields
    transaction->set_command(tlm::TLM_WRITE_COMMAND);
    transaction->set_address(address);
    transaction->set_data_ptr(reinterpret_cast<unsigned char*>(data));
    transaction->set_data_length(data_length); //In Bytes
    transaction->set_streaming_width(data_length);
    transaction->set_byte_enable_ptr(0);
    transaction->set_dmi_allowed(false); //Mandatory Initial Value
    transaction->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE); //Mandatory Initial Value

    //-----------DEBUG-----------
    printf("[DEBUG] Writing: ");
    for (int i = 0; i < data_length/sizeof(int); ++i){
      printf("%02x", *(data+i));
    }
    printf("\n");
    //-----------DEBUG-----------

    //Set transaction
    this->send_transaction(transaction);
  }
  
  void send_transaction(tlm::tlm_generic_payload*& transaction) {

    //Transaction Management Variables
    tlm::tlm_phase phase;
    tlm::tlm_sync_enum status;

    //Begin Request
    phase = tlm::BEGIN_REQ;
    transaction->acquire();
    cout << name() << " BEGIN_REQ SENT" << " TRANS ID " << 0 << " at time " << sc_time_stamp() << endl;
    pending_transaction = transaction;
    status = socket->nb_transport_fw(*transaction, phase, this->write_delay);  // Non-blocking transport call   

    // Check request status returned by target
    switch (status) {   
        //Case 1: Transaction was accepted
        case tlm::TLM_ACCEPTED: {
          printf("%s:\t %s received -> Transaction ID %d at time %s\n", name(), "TLM_ACCEPTED", 0, sc_time_stamp());
          //cout << name() << " TLM_ACCEPTED RECEIVED " << " TRANS ID " << transaction->transaction_id << " at time " << sc_time_stamp() << endl;
          check_transaction(*transaction);
          transaction->release();
          pending_transaction = 0;
          //Initiator only cares about sending the transaction, doesnt need to wait for response (non-blocking)
          break;
        }

        //Not implementing Updated and Completed Status
        default: {
          printf("%s:\t [ERROR] Invalid status received at initiator", name());
          break;
        }
    }

    //Wait for response transaction
    wait(transaction_received_e);
    //-----------DEBUG-----------
    printf("[DEBUG1] Reading at Initiator: ");
    for (int i = 0; i < this->data_length/sizeof(int); ++i){
    printf("%02x", *(reinterpret_cast<int*>(this->data)+i));
    }
    printf("\n");
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

    //printf("%s:\t %s received -> Transaction ID %d from address %x at time %s\n", name(), phase, this->id_extension->transaction_id, trans.get_address(), sc_time_stamp());
    //cout << name() << " " <<hex << trans.get_address() << " BEGIN_RESP RECEIVED at " << sc_time_stamp() << endl;
    switch (phase) {
      case tlm::BEGIN_RESP: {

        trans.acquire();
        this->data_length = trans.get_data_length();
        this->data = new unsigned char[this->data_length];
        memcpy(this->data, trans.get_data_ptr(), this->data_length);

        this->pending_transaction = &trans; //Set response transaction to return
        check_transaction(trans);

        //Initiator dont care about confirming resp transaction. So nothing else to do.

        //-----------DEBUG-----------
        printf("[DEBUG] Reading at Initiator: ");
        for (int i = 0; i < trans.get_data_length()/sizeof(int); ++i){
        printf("%02x", *(reinterpret_cast<int*>(trans.get_data_ptr())+i));
        }
        printf("\n");
        //-----------DEBUG-----------

        transaction_received_e.notify();
        //-----------DEBUG-----------
        printf("[DEBUG] Reading at Initiator: ");
        for (int i = 0; i < this->data_length/sizeof(int); ++i){
        printf("%02x", *(reinterpret_cast<int*>(this->data)+i));
        }
        printf("\n");
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
} ; 

#endif
