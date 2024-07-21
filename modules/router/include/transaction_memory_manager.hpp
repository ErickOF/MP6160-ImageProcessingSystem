// **************************************************************************************
// User-defined memory manager, which maintains a pool of transactions
// From TLM Duolos tutorials
// **************************************************************************************
#ifndef TRANSACTION_MEMORY_MANAGER_HPP
#define TRANSACTION_MEMORY_MANAGER_HPP

#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

typedef tlm::tlm_generic_payload gp_t;

class mm : public tlm::tlm_mm_interface
{
public:
    mm() : free_list(0), empties(0)
#ifdef DEBUG
    ,
    count(0)
#endif // DEBUG
    {
    }

    gp_t *allocate();
    void free(gp_t *trans);
private:
    struct access
    {
        gp_t *trans;
        access *next;
        access *prev;
    };

    access *free_list;
    access *empties;

#ifdef DEBUG
    int count;
#endif // DEBUG
};
#endif // TRANSACTION_MEMORY_MANAGER_HPP
