#ifndef IMG_TRANSMITER_HPP
#define IMG_TRANSMITER_HPP

#include <systemc.h>
#include "address_map.hpp"

SC_MODULE(img_transmiter)
{
    //Array for input image
    unsigned char* output_image;
    sc_dt::uint64 address_offset;

    SC_CTOR(img_transmiter)
    {
        output_image = new unsigned char[IMG_INPUT_SIZE];
        address_offset = IMG_OUTPUT_ADDRESS_LO;
    }
    
    //Backdoor access to memory
	void backdoor_write(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address);
	void backdoor_read(unsigned char*&data, unsigned int data_length, sc_dt::uint64 address);
};

#endif // IMG_TRANSMITER_HPP

