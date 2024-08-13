#ifndef IPS_VGA_TLM_HPP
#define IPS_VGA_TLM_HPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#include "common_func.hpp"
#include "important_defines.hpp"
#include "vga.hpp"
#include "../src/img_target.cpp"

// Extended Unification TLM
struct vga_tlm
  : public vga<
      IPS_BITS,
      IPS_H_ACTIVE, IPS_H_FP, IPS_H_SYNC_PULSE, IPS_H_BP,
      IPS_V_ACTIVE, IPS_V_FP, IPS_V_SYNC_PULSE, IPS_V_BP
    >, public img_target
{
protected:
  unsigned char* tmp_img;
public:
  vga_tlm(sc_module_name name)
    : vga<
      IPS_BITS,
      IPS_H_ACTIVE, IPS_H_FP, IPS_H_SYNC_PULSE, IPS_H_BP,
      IPS_V_ACTIVE, IPS_V_FP, IPS_V_SYNC_PULSE, IPS_V_BP>((std::string(name) + "_HW_block").c_str()),
      img_target((std::string(name) + "_target").c_str())
  {
#ifdef DISABLE_VGA_DEBUG
      this->use_prints = false;
#endif // DISABLE_VGA_DEBUG
      checkprintenableimgtar(use_prints);
  }

  // Override do_when_transaction functions
  virtual void do_when_read_transaction(unsigned char *&data, unsigned int data_length, sc_dt::uint64 address);
  virtual void do_when_write_transaction(unsigned char *&data, unsigned int data_length, sc_dt::uint64 address);
};
#endif // IPS_VGA_TLM_HPP