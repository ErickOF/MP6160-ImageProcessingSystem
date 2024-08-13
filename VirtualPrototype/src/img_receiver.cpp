#ifndef IMG_RECEIVER_CPP
#define IMG_RECEIVER_CPP

#include "img_receiver.hpp"
#include "common_func.hpp"

//Backdoor write/read for debug purposes
void img_receiver::backdoor_write(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  sc_dt::uint64 local_address = address - address_offset;
  memcpy((input_image + local_address), data, data_length);
  for (int i = 0; i < 10; i++) {
  	dbgmodprint("Backdoor Writing: %0d\n", *(input_image + local_address+i));
  }
}

void img_receiver::backdoor_read(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address)
{
  sc_dt::uint64 local_address = address - address_offset;
  data = new unsigned char[data_length];
  memcpy(data, (input_image + local_address), data_length);
  for (int i = 0; i < 10; i++) {
  	dbgmodprint("Backdoor Reading: %0d\n", *(input_image + local_address+i));
  }
}

#endif // IMG_RECEIVER_CPP

