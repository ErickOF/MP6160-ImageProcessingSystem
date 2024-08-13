#include "vga_tlm.hpp"

void vga_tlm::do_when_read_transaction(unsigned char *&data, unsigned int data_length, sc_dt::uint64 address)
{
  dbgimgtarmodprint(use_prints, "Calling do_when_read_transaction");

  if ((address >= IMG_INPUT_DONE_ADDRESS_LO) && (address < IMG_INPUT_DONE_ADDRESS_HI))
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
  else if ((address >= IMG_INPUT_ADDRESS_LO) && (address < IMG_INPUT_ADDRESS_HI))
  {
    memcpy(data, (vga::tmp_img + address - IMG_INPUT_ADDRESS_LOW), data_length);
  }
}

void vga_tlm::do_when_write_transaction(unsigned char *&data, unsigned int data_length, sc_dt::uint64 address)
{
  dbgimgtarmodprint(use_prints, "Calling do_when_write_transaction");

  if ((address >= IMG_INPUT_START_ADDRESS_LO) && (address < IMG_INPUT_START_ADDRESS_HI))
  {
    vga::start = (*data == 1);
    vga::done = !vga::start;
  }
}
