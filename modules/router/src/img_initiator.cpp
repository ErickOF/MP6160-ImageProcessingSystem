#include "img_initiator.hpp"

// Method to send_reading transaction and wait for response
void img_initiator::read(unsigned char *&data, unsigned int address, unsigned int data_length)
{
  // Create transaction and allocate it
  tlm::tlm_generic_payload *transaction = memory_manager.allocate();
  transaction->acquire();
  img_generic_extension *img_ext = new img_generic_extension;
  img_ext->transaction_number = this->transaction_number++;

  // Set transaction fields
  transaction->set_command(tlm::TLM_READ_COMMAND);
  transaction->set_address(address);
  transaction->set_data_ptr(data);
  // In Bytes
  transaction->set_data_length(data_length);
  transaction->set_streaming_width(data_length);
  transaction->set_byte_enable_ptr(0);
  // Mandatory Initial Value
  transaction->set_dmi_allowed(false);
  // Mandatory Initial Value
  transaction->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE);
  transaction->set_extension(img_ext);

  // Send transaction
  this->send_transaction(transaction);

  data = transaction->get_data_ptr();
  //-----------DEBUG-----------
  dbgmodprint("Reading at Initiator: ");

  for (int i = 0; i < transaction->get_data_length() / sizeof(int); ++i)
  {
    dbgmodprint("%02x", *(reinterpret_cast<int *>(transaction->get_data_ptr()) + i));
  }

  printf("\n");

  //-----------DEBUG-----------
}

void img_initiator::write(unsigned char *&data, unsigned int address, unsigned int data_length)
{
  // Create transaction and allocate it
  tlm::tlm_generic_payload *transaction = memory_manager.allocate();
  img_generic_extension *img_ext = new img_generic_extension;
  img_ext->transaction_number = this->transaction_number++;

  // Set transaction fields
  transaction->set_command(tlm::TLM_WRITE_COMMAND);
  transaction->set_address(address);
  transaction->set_data_ptr(data);
  transaction->set_data_length(data_length); // In Bytes
  transaction->set_streaming_width(data_length);
  transaction->set_byte_enable_ptr(0);
  transaction->set_dmi_allowed(false);                            // Mandatory Initial Value
  transaction->set_response_status(tlm::TLM_INCOMPLETE_RESPONSE); // Mandatory Initial Value
  transaction->set_extension(img_ext);

  //-----------DEBUG-----------
  dbgmodprint("Writing: ");

  for (int i = 0; i < data_length / sizeof(int); ++i)
  {
    dbgmodprint("%02x", *(reinterpret_cast<int *>(transaction->get_data_ptr()) + i));
  }

  printf("\n");
  //-----------DEBUG-----------

  // Set transaction
  this->send_transaction(transaction);
}

void img_initiator::send_transaction(tlm::tlm_generic_payload *&transaction)
{
  // Transaction Management Variables
  tlm::tlm_phase phase;
  tlm::tlm_sync_enum status;
  tlm::tlm_command cur_command;
  img_generic_extension *img_ext;

  // Begin Request
  phase = tlm::BEGIN_REQ;
  transaction->get_extension(img_ext);
  cur_command = transaction->get_command();

  this->transaction_sent_id = img_ext->transaction_number;

  dbgmodprint("BEGIN_REQ SENT TRANS ID %0d", img_ext->transaction_number);
  // Non-blocking transport call
  status = socket->nb_transport_fw(*transaction, phase, ((cur_command == tlm::TLM_WRITE_COMMAND) ? this->write_delay : this->read_delay));

  // Check request status returned by target
  switch (status)
  {
    // Case 1: Transaction was accepted
    case tlm::TLM_ACCEPTED:
    {
      dbgmodprint("%s received -> Transaction ID %d", "TLM_ACCEPTED", img_ext->transaction_number);
      check_transaction(*transaction);
      // transaction->release();
      // Initiator only cares about sending the transaction, doesnt need to wait for response (non-blocking)
      break;
    }

    // Not implementing Updated and Completed Status
    default:
    {
      dbgmodprint("[ERROR] Invalid status received at initiator -> Transaction ID %d", img_ext->transaction_number);
      break;
    }
  }

  // Wait for response transaction
  //  if (transaction->get_command() == tlm::TLM_READ_COMMAND) {
  wait(transaction_received_e);
  this->transaction_received_id = img_ext->transaction_number;
  // }
  //-----------DEBUG-----------
  dbgmodprint("[DEBUG1] Reading at Initiator: ");

  for (int i = 0; i < transaction->get_data_length() / sizeof(int); ++i)
  {
    dbgmodprint("%02x", *(reinterpret_cast<int *>(transaction->get_data_ptr()) + i));
  }

  printf("\n");
  //-----------DEBUG-----------

  // Increment transaction ID
}

tlm::tlm_sync_enum img_initiator::nb_transport_bw(
  tlm::tlm_generic_payload &trans,
  tlm::tlm_phase &phase, sc_time &delay
)
{
  // Call event queue
  m_peq.notify(trans, phase, delay);
  return tlm::TLM_ACCEPTED;
}

void img_initiator::peq_cb(tlm::tlm_generic_payload &trans, const tlm::tlm_phase &phase)
{
  // dbgmodprint("%s received -> Transaction ID %d from address %x", phase, this->id_extension->transaction_id);
  // cout << name() << " " <<hex << trans.get_address() << " BEGIN_RESP RECEIVED at " << sc_time_stamp() << endl;
  switch (phase)
  {
    case tlm::BEGIN_RESP:
    {
      check_transaction(trans);

      // Initiator dont care about confirming resp transaction. So nothing else to do.

      //-----------DEBUG-----------
      dbgmodprint("[DEBUG] Reading at Initiator: ");

      for (int i = 0; i < trans.get_data_length() / sizeof(int); ++i)
      {
        dbgmodprint("%02x", *(reinterpret_cast<int *>(trans.get_data_ptr()) + i));
      }

      printf("\n");
      //-----------DEBUG-----------

      transaction_received_e.notify();
      //-----------DEBUG-----------
      dbgmodprint("[DEBUG] Reading at Initiator: ");

      for (int i = 0; i < trans.get_data_length() / sizeof(int); ++i)
      {
        dbgmodprint("%02x", *(reinterpret_cast<int *>(trans.get_data_ptr()) + i));
      }

      printf("\n");
      //-----------DEBUG-----------
      break;
    }
    default:
    {
      SC_REPORT_FATAL("TLM-2", "Illegal transaction phase received by initiator");
      break;
    }
  }
}

void img_initiator::check_transaction(tlm::tlm_generic_payload &trans)
{
  // Check transaction if here

  // tlm::tlm_command command  = trans.get_command();
  // sc_dt::uint64    address  = trans.get_address();
  // unsigned char*   data_ptr = reinterpret_cast<unsigned char*>(trans.get_data_ptr());

  // Allow the memory manager to free the transaction object
  // trans.release();
}

void img_initiator::start_img_initiators()
{
  transaction_number = 0;
}

void img_initiator::set_delays(sc_time w_delay, sc_time r_delay)
{
  this->write_delay = w_delay;
  this->read_delay = r_delay;
}
