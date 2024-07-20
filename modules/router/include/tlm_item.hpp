#ifndef TLM_ITEM_HPP
#define TLM_ITEM_HPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

struct tlm_item
{
  tlm::tlm_generic_payload *transaction;
  tlm::tlm_phase phase;
  sc_time delay;
  tlm_item(tlm::tlm_generic_payload *transaction = 0, tlm::tlm_phase phase = tlm::BEGIN_REQ, sc_time delay = sc_time(0, SC_NS)) {}
  tlm_item &operator=(const tlm_item &rhs)
  {
    transaction = rhs.transaction;
    phase = rhs.phase;
    delay = rhs.delay;
    return *this;
  }

  bool operator==(const tlm_item &rhs)
  {
    return transaction == rhs.transaction && phase == rhs.phase && delay == rhs.delay;
  }

  friend std::ostream &operator<<(std::ostream &os, const tlm_item &val)
  {
    os << "Transaction Pointer = " << val.transaction << "; phase = " << val.phase << "; delay = " << val.delay << std::endl;
    return os;
  }
};
// inline void sc_trace(sc_trace_file*& f, const tlm_item& val, std::string name) {
//     sc_trace(f, val.transaction, name + ".transaction");
//     sc_trace(f, val.phase, name + ".phase");
//     sc_trace(f, val.delay, name + ".delay");
// }
#endif // TLM_ITEM_HPP
