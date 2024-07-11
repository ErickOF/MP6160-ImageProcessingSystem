#ifndef IMG_GENERIC_EXTENSION_CPP
#define IMG_GENERIC_EXTENSION_CPP
#include "systemc.h"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

#include "img_generic_extension.hpp"

tlm::tlm_extension_base* img_generic_extension::clone() const
{
  img_generic_extension* t = new img_generic_extension;
  t->transaction_number = this->transaction_number;
  return t;
}

void img_generic_extension::copy_from(tlm::tlm_extension_base const &ext)
{
  transaction_number = static_cast<img_generic_extension const &>(ext).transaction_number;
}

#endif // IMG_GENERIC_EXTENSION_CPP
