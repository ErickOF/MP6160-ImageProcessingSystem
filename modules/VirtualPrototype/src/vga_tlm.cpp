#include "vga_tlm.hpp"

void vga_tlm::do_when_read_transaction(unsigned char *&data, unsigned int data_length, sc_dt::uint64 address)
{
  dbgimgtarmodprint(use_prints, "Called do_when_read_transaction with an address %016llX and length %d", address, data_length);

  if ((address >= IMG_INPUT_SIZE + IMG_INPUT_START_SIZE) && (address < IMG_INPUT_SIZE + IMG_INPUT_START_SIZE + IMG_INPUT_DONE_SIZE))
  {
    if (vga::done == true)
    {
      *data = 1;
    }
    else
    {
      *data = 0;
      dbgimgtarmodprint(true, "VGA is still in progress");
    }
  }
  else if (address < IMG_INPUT_SIZE)
  {
    memcpy(data, (vga::tmp_img + address), data_length);
  }
}

void vga_tlm::do_when_write_transaction(unsigned char *&data, unsigned int data_length, sc_dt::uint64 address)
{
  dbgimgtarmodprint(use_prints, "Called do_when_write_transaction with an address %016llX and length %d", address, data_length);

  if ((address >= IMG_INPUT_SIZE) && (address < IMG_INPUT_SIZE+IMG_INPUT_START_SIZE))
  {
    dbgimgtarmodprint(true, "Start receiving data from VGA");
    vga::start = (*data == 1);
    vga::done = !vga::start;
  }
}

//Backdoor write/read for debug purposes
void vga_tlm::backdoor_write(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  sc_dt::uint64 local_address = address - IMG_INPUT_ADDRESS_LO;
  memcpy((vga::tmp_img + local_address), data, data_length);
  for (int i = 0; (i < 10) && (local_address + i < IMG_INPUT_SIZE); i++) {
  	dbgimgtarmodprint(true, "Backdoor Writing: %0d\n", *(vga::tmp_img + local_address + i));
  }
}

void vga_tlm::backdoor_read(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  sc_dt::uint64 local_address = address - IMG_INPUT_ADDRESS_LO;
  data = new unsigned char[data_length];
  memcpy(data, (vga::tmp_img + local_address), data_length);
  for (int i = 0; (i < 10) && (local_address + i < IMG_INPUT_SIZE); i++) {
  	dbgimgtarmodprint(true, "Backdoor Reading: %0d\n", *(vga::tmp_img + local_address + i));
  }
}

unsigned char *vga_tlm::return_mem_ptr()
{
  return vga::tmp_img;
}
