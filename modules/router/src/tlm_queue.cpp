#ifndef TLM_QUEUE_CPP
#define TLM_QUEUE_CPP

#include <systemc.h>
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"


#include "common_func.hpp"
#include "img_generic_extension.hpp"

typedef struct tlm_item{
    tlm::tlm_generic_payload *transaction;
    tlm::tlm_phase phase;
    sc_time delay;
} tlm_item_t;

template<unsigned int QUEUE_DEPTH>
struct tlm_queue : sc_module
{
    tlm_item_t tlm_item_queue[QUEUE_DEPTH];
    unsigned int tlm_queue_ptr;

    bool is_full;
    bool is_empty;

    sc_event item_pushed_e;
    sc_event item_popped_e;

    SC_CTOR(tlm_queue): is_full(false), is_empty(true), tlm_queue_ptr(0) {
        // for (unsigned int i=0; i < QUEUE_DEPTH; i++){
        //     tlm_item_queue[i];
        // }

        //SC_THREAD(tlm_queue_thread);
    }

    void push(tlm::tlm_generic_payload *trans,   
              tlm::tlm_phase& phase, sc_time& delay) {
        if (is_full) {
            printf("[FULL] Waiting for queue to pop..\n");
            wait(item_popped_e);
        }
        img_generic_extension* img_ext;
        (*trans).get_extension(img_ext);
        printf("[PUSH] pushing transaction %0d to queue index %0d\n",  img_ext->transaction_number, tlm_queue_ptr);
        
        tlm_item_queue[tlm_queue_ptr].transaction = trans;
        tlm_item_queue[tlm_queue_ptr].phase = phase;
        tlm_item_queue[tlm_queue_ptr].delay = delay;
        tlm_queue_ptr++;
        is_full = (QUEUE_DEPTH <= tlm_queue_ptr);
        item_pushed_e.notify();
    }

    void pop(tlm::tlm_generic_payload *trans,   
                       tlm::tlm_phase& phase, sc_time& delay) {
        if (is_empty) {
            printf("[EMPTY] Waiting for queue to push..\n");
            wait(item_pushed_e);
        }

        trans = tlm_item_queue[tlm_queue_ptr].transaction; 
        phase = tlm_item_queue[tlm_queue_ptr].phase;
        delay = tlm_item_queue[tlm_queue_ptr].delay;
        img_generic_extension* img_ext;
        (*trans).get_extension(img_ext);
        printf("[PULL] pushing transaction %0d to queue index %0d\n",  img_ext->transaction_number, tlm_queue_ptr);
        
        
        // //tlm_queue_ptr--;
        // is_empty = (0 == tlm_queue_ptr);
        // item_popped_e.notify();               
    }


};



#endif //TLM_QUEUE_CPP