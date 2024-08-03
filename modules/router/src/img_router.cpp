

#ifndef IMG_ROUTER_CPP
#define IMG_ROUTER_CPP

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
#include <ostream>

#include "common_func.hpp"
#include "img_generic_extension.hpp"

//#include "tlm_queue.cpp"

#include "important_defines.hpp"

//const char* tlm_enum_names[] = {"TLM_ACCEPTED", "TLM_UPDATED", "TLM_COMPLETED"};

struct tlm_item{
    tlm::tlm_generic_payload *transaction;
    tlm::tlm_phase phase;
    sc_time delay;
    tlm_item(tlm::tlm_generic_payload *transaction = 0, tlm::tlm_phase phase = tlm::BEGIN_REQ, sc_time delay = sc_time(0, SC_NS)){}
    tlm_item& operator=(const tlm_item& rhs){
        transaction = rhs.transaction;
        phase = rhs.phase;
        delay = rhs.delay;
        return *this;
    }
    bool operator==(const tlm_item& rhs){
        return transaction == rhs.transaction && phase == rhs.phase && delay == rhs.delay;
    }
    friend std::ostream& operator<<(std::ostream& os, const tlm_item& val) {
        os << "Transaction Pointer = " << val.transaction << "; phase = " << val.phase << "; delay = " << val.delay << std::endl;
        return os;
    }
};
// inline void sc_trace(sc_trace_file*& f, const tlm_item& val, std::string name) {
//     sc_trace(f, val.transaction, name + ".transaction");
//     sc_trace(f, val.phase, name + ".phase");
//     sc_trace(f, val.delay, name + ".delay");
// }

// Initiator module generating generic payload transactions
template<unsigned int N_TARGETS>
struct img_router: sc_module 
{   
  // TLM2.0 Socket  
  tlm_utils::simple_target_socket<img_router> target_socket;
  tlm_utils::simple_initiator_socket<img_router>* initiator_socket[N_TARGETS];

  //Memory Manager for transaction memory allocation
  mm memory_manager;

  //Payload event queue with callback and phase
  tlm_utils::peq_with_cb_and_phase<img_router> bw_m_peq; //For initiator access
  tlm_utils::peq_with_cb_and_phase<img_router> fw_m_peq; //For target access

  //Delay
  sc_time bw_delay;
  sc_time fw_delay;

  //TLM Items queue
  sc_fifo<tlm_item> fw_fifo;
  sc_fifo<tlm_item> bw_fifo;

  //DEBUG
  unsigned int transaction_in_fw_path_id = 0;
  unsigned int transaction_in_bw_path_id = 0;
  
  //Constructor
  SC_CTOR(img_router)   
  : target_socket("socket"), bw_m_peq(this, &img_router::bw_peq_cb), fw_m_peq(this, &img_router::fw_peq_cb), fw_fifo(10), bw_fifo(2) // Construct and name socket   
  {   
    // Register callbacks for incoming interface method calls
    target_socket.register_nb_transport_fw(this, &img_router::nb_transport_fw);
    for (unsigned int i = 0; i < N_TARGETS; i++) {
        char txt[20];
        sprintf(txt, "socket_%d", i);
        initiator_socket[i] = new tlm_utils::simple_initiator_socket<img_router>(txt);
        (*initiator_socket[i]).register_nb_transport_bw(this, &img_router::nb_transport_bw);
    }

    SC_THREAD(fw_thread);
    SC_THREAD(bw_thread);
  }

  //Address Decoding
  #define IMG_FILTER_INITIATOR_ID 0
  #define IMG_SOBEL_INITIATOR_ID  1
  #define IMG_MEMORY_INITIATOR_ID 2
  #define INVALID_INITIATOR_ID    3
  
  unsigned int decode_address (sc_dt::uint64 address)
  {
    switch(address) {
        // To Filter
        case IMG_FILTER_KERNEL: {
            dbgmodprint("Decoded address %016llX corresponds to Filter module.", address);
            return IMG_FILTER_INITIATOR_ID;
        }

        // To/from Sobel
        case SOBEL_INPUT_0: 
        case SOBEL_INPUT_1: 
        case SOBEL_OUTPUT: {
            dbgmodprint("Decoded address %016llX corresponds to Sobel module.", address);
            return IMG_SOBEL_INITIATOR_ID;
        }

        // To/From Memory Valid addresses
        case MEM_START ... MEM_FINISH : {
            dbgmodprint("Decoded address %016llX corresponds to Memory.", address);
            return IMG_MEMORY_INITIATOR_ID;
        }
        default: {
            dbgmodprint("[ERROR] Decoding invalid address %016llX.", address);
            SC_REPORT_FATAL("[IMG ROUTER]", "Received address is invalid, does not match any hardware block");
            return INVALID_INITIATOR_ID;
        }   
    }
  }
   
  // TLM2 backward path non-blocking transport method
  virtual tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload& trans,   
                                             tlm::tlm_phase& phase, sc_time& delay )   
  { 
    img_generic_extension* img_ext;
    
    //Call event queue
    tlm_item item;
    item.transaction = &trans;
    item.phase = phase;
    item.delay = delay;
    trans.get_extension(img_ext);
    if (bw_fifo.num_free() == 0) {
        dbgmodprint("[BW_FIFO] FIFO is FULL. Waiting...");
        wait(bw_fifo.data_read_event());
    }
    bw_fifo.nb_write(item);
    wait(bw_fifo.data_written_event());
    dbgmodprint("[BW_FIFO] Pushed transaction #%0d", img_ext->transaction_number);
    return tlm::TLM_ACCEPTED;
  }

    // TLM2 forward path non-blocking transport method
  virtual tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans,   
                                             tlm::tlm_phase& phase, sc_time& delay )   
  { 
    img_generic_extension* img_ext;
    
    //Call event queue
    tlm_item item;
    item.transaction = &trans;
    item.phase = phase;
    item.delay = delay;
    trans.get_extension(img_ext);
    if (fw_fifo.num_free() == 0) {
        dbgmodprint("[FW_FIFO] FIFO is FULL. Waiting...");
        wait(fw_fifo.data_read_event());
    }
    fw_fifo.nb_write(item);
    wait(fw_fifo.data_written_event());
    dbgmodprint("[FW_FIFO] Pushed transaction #%0d", img_ext->transaction_number);
    return tlm::TLM_ACCEPTED;
  }

  void fw_thread() {
    while(true) { 
        img_generic_extension* img_ext;
        tlm::tlm_generic_payload* trans_ptr = new tlm::tlm_generic_payload;
        tlm::tlm_phase phase;
        sc_time delay;

        tlm_item item;
        if (fw_fifo.num_available() == 0) {
            wait(fw_fifo.data_written_event());
        }
        fw_fifo.nb_read(item);
        wait(fw_fifo.data_read_event());
        trans_ptr = item.transaction;
        phase = item.phase;
        delay = item.delay;
        (*trans_ptr).get_extension(img_ext);
        dbgmodprint("[FW_FIFO] Popped transaction #%0d", img_ext->transaction_number);
        fw_m_peq.notify(*trans_ptr, phase, delay);
        wait(fw_delay);
    }
  }

  void bw_thread() {
    while(true) { 
        img_generic_extension* img_ext;
        tlm::tlm_generic_payload* trans_ptr = new tlm::tlm_generic_payload;
        tlm::tlm_phase phase;
        sc_time delay;

        tlm_item item;
        if (bw_fifo.num_available() == 0) {
            wait(bw_fifo.data_written_event());
        }
        bw_fifo.nb_read(item);
        wait(bw_fifo.data_read_event());
        trans_ptr = item.transaction;
        phase = item.phase;
        delay = item.delay;
        (*trans_ptr).get_extension(img_ext);
        dbgmodprint("[BW_FIFO] Popped transaction #%0d", img_ext->transaction_number);
        bw_m_peq.notify(*trans_ptr, phase, delay);
        wait(bw_delay);
    }
  }

  //Payload event and queue callback to handle transactions received from target
  void bw_peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase)
  {
    img_generic_extension* img_ext;
    trans.get_extension(img_ext);
    tlm::tlm_phase local_phase = phase;

    sc_dt::uint64 address = trans.get_address();
    this->transaction_in_bw_path_id = img_ext->transaction_number;

    dbgmodprint("Received transaction #%0d with address %016llX in backward path. Redirecting transaction to CPU", img_ext->transaction_number, address);
    target_socket->nb_transport_bw(trans, local_phase, this->bw_delay);
  }

  void fw_peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase)
  {
    img_generic_extension* img_ext;
    trans.get_extension(img_ext);
    tlm::tlm_phase local_phase = phase;

    sc_dt::uint64 address = trans.get_address();
    this->transaction_in_fw_path_id = img_ext->transaction_number;

    unsigned int initiator_id = decode_address(address);
    dbgmodprint("Received transaction #%0d with address %016llX in forward path. Redirecting transaction through initiator %d", img_ext->transaction_number, address, initiator_id);
    (*initiator_socket[initiator_id])->nb_transport_fw(trans, local_phase, this->fw_delay);
  }
  
  void set_delays(sc_time fw_delay, sc_time bw_delay)
  {
    this->bw_delay = bw_delay;
    this->fw_delay = fw_delay;
  }
} ; 

#endif
