#ifndef IMG_RECEIVER_HPP
#define IMG_RECEIVER_HPP

#include <systemc.h>
#include "address_map.hpp"

SC_MODULE(img_receiver)
{
    //Array for input image
    unsigned char* input_image;
    sc_dt::uint64 address_offset;

    SC_CTOR(img_receiver)
    {
        input_image = new unsigned char[IMG_INPUT_SIZE];
        address_offset = IMG_INPUT_ADDRESS_LO;
    }
    
    //Backdoor access to memory
	void backdoor_write(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address);
	void backdoor_read(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address);
};

#endif // IMG_RECEIVER_HPP

