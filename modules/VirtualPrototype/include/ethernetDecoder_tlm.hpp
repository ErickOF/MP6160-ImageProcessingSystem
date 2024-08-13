#ifndef ETHERNET_DECODER_TLM_H
#define ETHERNET_DECODER_TLM_H

#include <systemc-ams.h>

#include "important_defines.hpp"
#include "ethernetDecoder.h"

struct ethernetDecoder_tlm : public ethernetDecoder
{
  void processing() override;

  ethernetDecoder_tlm(sc_module_name name, sca_core::sca_time sample_time) : ethernetDecoder((std::string(name) + "_AMS_HW_block").c_str(), sample_time)
  {
    data = new unsigned char[IMAG_ROWS * IMAG_COLS];
  }

  unsigned char *data;

  sc_dt::sc_bv<8> decoded_byte;
};

#endif // ETHERNET_DECODER_TLM_H
