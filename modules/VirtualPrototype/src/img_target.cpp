#ifndef IMG_TARGET_CPP
#define IMG_TARGET_CPP

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_target_socket.h>

#include "common_func.hpp"

// Initiator module generating generic payload transactions   
struct img_target: sc_module 
{   

    // TLM2.0 Socket  
    tlm_utils::simple_target_socket<img_target> socket;


	const sc_core::sc_time LATENCY;

    //DEBUG
    unsigned int transaction_in_progress_id = 0;

    uint32_t address_offset = 0;
    int mem_size = 0;
    

    //Constructor
    SC_CTOR(img_target)   
    : socket("socket"), LATENCY(sc_time(10, SC_NS)) // Construct and name socket   
    {   
        // Register callbacks for incoming interface method calls
        socket.register_b_transport(this, &img_target::b_transport);

    }

    virtual void do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address){}
    virtual void do_when_write_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address){}

    void set_mem_attributes(uint32_t address_offset, int mem_size)
    {
        this->address_offset = address_offset;
        this->mem_size = mem_size;
    }

    void b_transport(tlm::tlm_generic_payload &trans, sc_core::sc_time &delay) {
        tlm::tlm_command cmd = trans.get_command();
        sc_dt::uint64 adr = trans.get_address();
        unsigned char *ptr = trans.get_data_ptr();
        unsigned int len = trans.get_data_length();
        unsigned char *byt = trans.get_byte_enable_ptr();
        unsigned int wid = trans.get_streaming_width();

	    adr = adr - address_offset;

        // Obliged to check address range and check for unsupported features,
        //   i.e. byte enables, streaming, and bursts
        // Can ignore extensions

        // *********************************************
        // Generate the appropriate error response
        // *********************************************
        if (adr >= sc_dt::uint64(mem_size)) {
            dbgmodprint("[DEBUG ERROR] TLM transaction is returned with response status TLM_ADDRESS_ERROR_RESPONSE");
            trans.set_response_status(tlm::TLM_ADDRESS_ERROR_RESPONSE);
            return;
        }
        if (byt != nullptr) {
            dbgmodprint("[DEBUG ERROR] TLM transaction is returned with response status TLM_BYTE_ERROR_RESPONSE");
            trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
            return;
        }
        if (len > 4 || wid < len) {
            dbgmodprint("[DEBUG ERROR] TLM transaction is returned with response status TLM_BURST_ERROR_RESPONSE");
            trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
            return;
        }

        // Obliged to implement read and write commands
        if (cmd == tlm::TLM_READ_COMMAND) {
            this->do_when_read_transaction(ptr, len, adr);
            // dbgmodprint("Read at address %0x, data: %0x", address_offset, *ptr);
        }
        else if (cmd == tlm::TLM_WRITE_COMMAND) {
            // dbgmodprint("Write at address %0x, data: %0x", address_offset, *ptr);
            this->do_when_write_transaction(ptr, len, adr);
        }

        // Illustrates that b_transport may block
        //sc_core::wait(delay);

        // Reset timing annotation after waiting
        delay = sc_core::SC_ZERO_TIME;

        // *********************************************
        // Set DMI hint to indicated that DMI is supported
        // *********************************************

        if (address_offset == 0) {
            trans.set_dmi_allowed(true);
        } else {
            trans.set_dmi_allowed(false);
        }

        // Obliged to set response status to indicate successful completion
        trans.set_response_status(tlm::TLM_OK_RESPONSE);
    }
};
#endif
