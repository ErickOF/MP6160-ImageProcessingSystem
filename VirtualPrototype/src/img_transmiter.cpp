#ifndef IMG_TRANSMITER_CPP
#define IMG_TRANSMITER_CPP

#include "img_transmiter.hpp"
#include "common_func.hpp"

//Backdoor write/read for debug purposes
void img_transmiter::backdoor_write(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  sc_dt::uint64 local_address = address - address_offset;
  memcpy((output_image + local_address), data, data_length);
  for (int i = 0; i < 10; i++) {
  	dbgmodprint("Backdoor Writing: %0d\n", *(output_image + local_address+i));
  }
}

void img_transmiter::backdoor_read(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  sc_dt::uint64 local_address = address - address_offset;
  data = new unsigned char[data_length];
  memcpy(data, (output_image + local_address), data_length);
  for (int i = 0; i < 10; i++) {
  	dbgmodprint("Backdoor Reading: %0d\n", *(output_image + local_address+i));
  }
}

#endif // IMG_TRANSMITER_CPP

