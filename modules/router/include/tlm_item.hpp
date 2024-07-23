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

  tlm_item() : transaction(nullptr)
  {
  }

  tlm_item(tlm::tlm_generic_payload *transaction, tlm::tlm_phase phase, sc_time delay)
  {
    this->transaction = transaction;
    this->phase = phase;
    this->delay = delay;
  }

  tlm_item &operator=(const tlm_item &rhs)
  {
    if (this != &rhs)
    {
      this->transaction = rhs.transaction;
      this->phase = rhs.phase;
      this->delay = rhs.delay;
    }

    return *this;
  }

  tlm_item(const tlm_item &other) : transaction(other.transaction)
  {
    this->phase = other.phase;
    this->delay = other.delay;
  }

  bool operator==(const tlm_item &rhs)
  {
    return this->transaction == rhs.transaction && this->phase == rhs.phase && this->delay == rhs.delay;
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
