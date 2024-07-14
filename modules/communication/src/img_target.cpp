#ifndef IMG_TARGET_CPP
#define IMG_TARGET_CPP

#include "systemc.h"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

//For an internal response phase
DECLARE_EXTENDED_PHASE(internal_processing_ph);


// Initiator module generating generic payload transactions   
struct img_target: sc_module 
{   

    // TLM2.0 Socket  
    tlm_utils::simple_target_socket<img_target> socket;

    //Internal fields
    unsigned char* data_ptr;
    unsigned int data_length;
    unsigned int address;

    //Pointer to transaction in progress
    tlm::tlm_generic_payload* response_transaction; 

    //Payload event queue with callback and phase
    tlm_utils::peq_with_cb_and_phase<img_target> m_peq;

    //Delay
    sc_time response_delay = sc_time(10, SC_NS);
    sc_time receive_delay = sc_time(10,SC_NS);

    //Constructor
    SC_CTOR(img_target)   
    : socket("socket"), response_transaction(0), m_peq(this, &img_target::peq_cb) // Construct and name socket   
    {   
        // Register callbacks for incoming interface method calls
        socket.register_nb_transport_fw(this, &img_target::nb_transport_fw);
    }

    tlm::tlm_sync_enum nb_transport_fw(tlm::tlm_generic_payload& trans,
                                       tlm::tlm_phase& phase, sc_time& delay)
    {
        if (trans.get_byte_enable_ptr() != 0) {
            trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
            return tlm::TLM_COMPLETED;
        }
        if (trans.get_streaming_width() < trans.get_data_length()) {
            trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
            return tlm::TLM_COMPLETED;
        }

        // Queue the transaction
        m_peq.notify(trans, phase, delay);
        return tlm::TLM_ACCEPTED;
    }

    //Payload and Queue Callback
    void peq_cb(tlm::tlm_generic_payload& trans, const tlm::tlm_phase& phase)
    {
        tlm::tlm_sync_enum status;
        sc_time delay;

        switch (phase) {
            //Case 1: Target is receiving the first transaction of communication -> BEGIN_REQ
            case tlm::BEGIN_REQ: {
                cout << name() << " BEGIN_REQ RECEIVED" << " TRANS ID " << 0 << " at time " << sc_time_stamp() << endl;      
                //Check for errors here

                // Increment the transaction reference count
                trans.acquire();
                
                //Queue a response
                tlm::tlm_phase int_phase = internal_processing_ph;
                m_peq.notify(trans, int_phase, response_delay);
                break;
            }
            case tlm::END_RESP:
            case tlm::END_REQ:
            case tlm::BEGIN_RESP:{
                SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by target");
                break;
            }
            default: {
                if (phase == internal_processing_ph){
                    cout << "INTERNAL PHASE: PROCESSING TRANSACTION" << endl;
                    process_transaction(trans);
                }
                break;
            }
        }
    }

    //Resposne function
    void send_response(tlm::tlm_generic_payload& trans)
    {
        tlm::tlm_sync_enum status;
        tlm::tlm_phase response_phase;

        response_phase = tlm::BEGIN_RESP;
        status = socket->nb_transport_bw(trans, response_phase, response_delay);
        
        //Check Initiator response
        switch(status) {
            case tlm::TLM_ACCEPTED: {
                cout << name() << " TLM_ACCEPTED RECEIVED" << " TRANS ID " << 0 <<  " at time " << sc_time_stamp() << endl;
                // Target only care about acknowledge of the succesful response
                trans.release();
                break;
            }

            //Not implementing Updated and Completed Status
            default: {
                printf("%s:\t [ERROR] Invalid status received at target", name());
                break;
            }
        }

    }

    virtual void do_when_read_transaction(unsigned char*& data){

    }
    virtual void do_when_write_transaction(unsigned char*&data){

    }

    //Thread to call nb_transport on the backward path -> here the module process data and responds to initiator
    void process_transaction(tlm::tlm_generic_payload& trans)
    {
        //Status and Phase
        tlm::tlm_sync_enum status;
        tlm::tlm_phase phase;

        cout << name() << " Processing transaction: " << 0 << endl;

        //get variables from transaction
        tlm::tlm_command cmd      = trans.get_command();   
        sc_dt::uint64    addr     = trans.get_address();   
        unsigned char*   data_ptr = trans.get_data_ptr();   
        unsigned int     len      = trans.get_data_length();   
        unsigned char*   byte_en  = trans.get_byte_enable_ptr();   
        unsigned int     width    = trans.get_streaming_width();

        //Process transaction
        switch(cmd) {
            case tlm::TLM_READ_COMMAND: {
                unsigned char* response_data_ptr;
                this->do_when_read_transaction(response_data_ptr);
                //Add read according to length
                //-----------DEBUG-----------
                printf("[DEBUG] Reading: ");
                for (int i = 0; i < len/sizeof(int); ++i){
                printf("%02x", *(reinterpret_cast<int*>(response_data_ptr)+i));
                }
                printf("\n");
                //-----------DEBUG-----------
                trans.set_data_ptr(response_data_ptr);
                break;
            }
            case tlm::TLM_WRITE_COMMAND: {
                this->do_when_write_transaction(data_ptr);
                //-----------DEBUG-----------
                printf("[DEBUG] Writing: ");
                for (int i = 0; i < len/sizeof(int); ++i){
                printf("%02x", *(reinterpret_cast<int*>(data_ptr)+i));
                }
                printf("\n");
                //-----------DEBUG-----------
                break;
            }
            default: {
                cout << " ERROR " << "Command " << cmd << "is NOT valid" << endl;
            }
        }

        //Send response
        cout << name() << " BEGIN_RESP SENT" << " TRANS ID " << 0 <<  " at time " << sc_time_stamp() << endl;
        send_response(trans);
    }
};
#endif
