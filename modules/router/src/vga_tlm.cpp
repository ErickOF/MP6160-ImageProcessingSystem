#include "vga_tlm.hpp"

void vga_tlm::do_when_read_transaction(unsigned char*& data, unsigned int data_length, sc_dt::uint64 address)
{
  dbgimgtarmodprint(use_prints, "Calling do_when_read_transaction");

  if ((address >= IMG_INPUT_ADDRESS_LO) && (address < IMG_INPUT_ADDRESS_HI))
  {
    if (vga::done == true)
    {
      memcpy(address, tmp_img, IMG_INPUT_SIZE);
    }
  }
}

void vga_tlm::do_when_write_transaction(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  dbgimgtarmodprint(use_prints, "Calling do_when_write_transaction");

  if ((address >= IMG_INPUT_ADDRESS_LO) && (address < IMG_INPUT_ADDRESS_HI))
  {
    vga::start = true;
  }
}
