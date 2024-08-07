#include "transaction_memory_manager.hpp"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

gp_t *mm::allocate()
{
#ifdef DEBUG
  cout << "----------------------------- Called allocate(), #trans = " << ++count << endl;
#endif // DEBUG
  gp_t *ptr;

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

void mm::free(gp_t *trans)
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
