#include "img_target.hpp"

tlm::tlm_sync_enum img_target::nb_transport_fw(
    tlm::tlm_generic_payload &trans,
    tlm::tlm_phase &phase, sc_time &delay)
{
    if (trans.get_byte_enable_ptr() != 0)
    {
        trans.set_response_status(tlm::TLM_BYTE_ENABLE_ERROR_RESPONSE);
        return tlm::TLM_COMPLETED;
    }

    if (trans.get_streaming_width() < trans.get_data_length())
    {
        trans.set_response_status(tlm::TLM_BURST_ERROR_RESPONSE);
        return tlm::TLM_COMPLETED;
    }

    // Queue the transaction
    m_peq.notify(trans, phase, delay);
    return tlm::TLM_ACCEPTED;
}

void img_target::peq_cb(tlm::tlm_generic_payload &trans, const tlm::tlm_phase &phase)
{
    tlm::tlm_sync_enum status;
    sc_time delay;
    img_generic_extension *img_ext;

    switch (phase)
    {
        // Case 1: Target is receiving the first transaction of communication -> BEGIN_REQ
        case tlm::BEGIN_REQ:
        {
            // Check for errors here

            // Increment the transaction reference count
            trans.acquire();
            trans.get_extension(img_ext);

            dbgmodprint("BEGIN_REQ RECEIVED TRANS ID %0d", img_ext->transaction_number);

            // Queue a response
            tlm::tlm_phase int_phase = internal_processing_ph;
            m_peq.notify(trans, int_phase, receive_delay);
            break;
        }
        case tlm::END_RESP:
        case tlm::END_REQ:
        case tlm::BEGIN_RESP:
        {
            SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by target");
            break;
        }
        default:
        {
            if (phase == internal_processing_ph)
            {
                dbgmodprint("INTERNAL PHASE: PROCESSING TRANSACTION");
                process_transaction(trans);
            }
            break;
        }
    }
}

void img_target::send_response()
{
    while (true)
    {
        wait(send_response_e);
        tlm::tlm_sync_enum status;
        tlm::tlm_phase response_phase;
        img_generic_extension *img_ext;

        response_phase = tlm::BEGIN_RESP;
        status = socket->nb_transport_bw(*response_transaction, response_phase, response_delay);

        // Check Initiator response
        switch (status)
        {
            case tlm::TLM_ACCEPTED:
            {
                // Target only care about acknowledge of the succesful response
                (*response_transaction).release();
                (*response_transaction).get_extension(img_ext);
                dbgmodprint("TLM_ACCEPTED RECEIVED TRANS ID %0d", img_ext->transaction_number);
                break;
            }

            // Not implementing Updated and Completed Status
            default:
            {
                dbgmodprint("[ERROR] Invalid status received at target");
                break;
            }
        }
    }
}

void img_target::process_transaction(tlm::tlm_generic_payload &trans)
{
    // Status and Phase
    tlm::tlm_sync_enum status;
    tlm::tlm_phase phase;
    img_generic_extension *img_ext;

    // get variables from transaction
    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64 addr = trans.get_address();
    unsigned char *data_ptr = trans.get_data_ptr();
    unsigned int len = trans.get_data_length();
    unsigned char *byte_en = trans.get_byte_enable_ptr();
    unsigned int width = trans.get_streaming_width();
    trans.get_extension(img_ext);

    dbgmodprint("Processing transaction: %0d", img_ext->transaction_number);
    this->transaction_in_progress_id = img_ext->transaction_number;

    // Process transaction
    switch (cmd)
    {
        case tlm::TLM_READ_COMMAND:
        {
            unsigned char *response_data_ptr;
            response_data_ptr = new unsigned char[len];
            this->do_when_read_transaction(response_data_ptr, len, addr);
            // Add read according to length
            //-----------DEBUG-----------
            dbgmodprint("[DEBUG] Reading: ");
            for (int i = 0; i < len / sizeof(int); ++i)
            {
                dbgmodprint("%02x", *(reinterpret_cast<int *>(response_data_ptr) + i));
            }
            printf("\n");
            //-----------DEBUG-----------
            trans.set_data_ptr(response_data_ptr);
            break;
        }
        case tlm::TLM_WRITE_COMMAND:
        {
            this->do_when_write_transaction(data_ptr, len, addr);
            //-----------DEBUG-----------
            dbgmodprint("[DEBUG] Writing: ");
            for (int i = 0; i < len / sizeof(int); ++i)
            {
                dbgmodprint("%02x", *(reinterpret_cast<int *>(data_ptr) + i));
            }
            printf("\n");
            //-----------DEBUG-----------
            break;
        }
        default:
        {
            dbgmodprint("ERROR Command %0d is NOT valid", cmd);
        }
    }

    // Send response
    dbgmodprint("BEGIN_RESP SENT TRANS ID %0d", img_ext->transaction_number);
    response_transaction = &trans;
    // send_response(trans);
    send_response_e.notify();
}

void img_target::set_delays(sc_time resp_delay, sc_time rec_delay)
{
    this->response_delay = resp_delay;
    this->receive_delay = rec_delay;
}
