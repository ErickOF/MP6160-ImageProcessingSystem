#ifndef IMG_GENERIC_EXTENSION_HPP
#define IMG_GENERIC_EXTENSION_HPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

struct img_generic_extension : tlm::tlm_extension<img_generic_extension>
{
  img_generic_extension() : transaction_number(0) {}
  
  virtual tlm::tlm_extension_base* clone() const;

  virtual void copy_from(tlm::tlm_extension_base const &ext);
  
  unsigned int transaction_number;
};

#endif // IMG_GENERIC_EXTENSION_HPP
