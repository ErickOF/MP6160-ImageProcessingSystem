// **************************************************************************************
// User-defined memory manager, which maintains a pool of transactions
// From TLM Duolos tutorials
// **************************************************************************************
#ifndef TRANSACTION_MEMORY_MANAGER_CPP
#define TRANSACTION_MEMORY_MANAGER_CPP

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

class mm: public tlm::tlm_mm_interface
{
  typedef tlm::tlm_generic_payload gp_t;

public:
  mm() : free_list(0), empties(0)
  #ifdef DEBUG
  , count(0)
  #endif // DEBUG
  {}

  gp_t* allocate()
  {
    #ifdef DEBUG
      cout << "----------------------------- Called allocate(), #trans = " << ++count << endl;
    #endif // DEBUG
    gp_t* ptr;
    if (free_list)
    {
      ptr = free_list->trans;
      empties = free_list;
      free_list = free_list->next;
    }
    else
    {
      ptr = new gp_t(this);
    }
    return ptr;
  }

  void  free(gp_t* trans)
  {
    #ifdef DEBUG
      cout << "----------------------------- Called free(), #trans = " << --count << endl;
    #endif // DEBUG
    if (!empties)
    {
      empties = new access;
      empties->next = free_list;
      empties->prev = 0;
      if (free_list)
        free_list->prev = empties;
    }
    free_list = empties;
    free_list->trans = trans;
    empties = free_list->prev;
  }

private:
  struct access
  {
    gp_t* trans;
    access* next;
    access* prev;
  };

  access* free_list;
  access* empties;

  #ifdef DEBUG
  int     count;
  #endif // DEBUG
};
#endif // TRANSACTION_MEMORY_MANAGER_CPP
