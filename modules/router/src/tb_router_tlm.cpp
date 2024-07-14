#ifdef USING_TLM_ROUTER_TB_EN
#ifndef TB_ROUTER_TLM_CPP
#define TB_ROUTER_TLM_CPP

#include <systemc.h>

#include <cmath>

using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

#include "img_initiator.cpp"
#include "img_target.cpp"
#include "img_router.cpp"

#include "common_func.hpp"
#include "ImportantDefines.h"

#define TEST_N_TARGETS 3

SC_MODULE(Tb_router_top)
{
  img_initiator *tb_initiator;
  img_target *tb_targets[TEST_N_TARGETS];
  img_router<TEST_N_TARGETS> *tb_router;
  
  SC_CTOR(Tb_router_top)
  {
    tb_initiator = new img_initiator("tb_initiator");
    tb_router = new img_router<TEST_N_TARGETS>("tb_router");

    for (unsigned int i = 0; i < TEST_N_TARGETS; i++) {
      char txt[20];
      sprintf(txt, "tb_target_%d", i);
      tb_targets[i] = new img_target(txt);
      tb_targets[i]->set_delays(sc_time(2, SC_NS), sc_time(2, SC_NS));
      tb_targets[i]->socket.bind(*tb_router->initiator_socket[i]);
    }
    
    tb_initiator->start_img_initiators();
    tb_initiator->set_delays(sc_time(2, SC_NS), sc_time(2, SC_NS));
    tb_router->set_delays(sc_time(10, SC_NS), sc_time(10, SC_NS));
    
    // Bind initiator socket to target socket
    tb_initiator->socket.bind(tb_router->target_socket);
    
    SC_THREAD(thread_process);
  }
  
  #define N_ADDRESSES 10
  void thread_process()
  {
    sc_time tb_send_delay = sc_time(0, SC_NS);
    unsigned int n_transactions = 10;
    sc_dt::uint64 address;
    sc_dt::uint64 address_list[N_ADDRESSES] = {IMG_FILTER_KERNEL,
                                               SOBEL_INPUT_0,
                                               SOBEL_INPUT_1,
                                               SOBEL_OUTPUT,
                                               IMG_INPUT,
                                               IMG_INPROCESS_A,
                                               IMG_INPROCESS_B,
                                               IMG_INPROCESS_C,
                                               IMG_INPROCESS_D,
                                               IMG_COMPRESSED};
    
    unsigned int data_length;
    unsigned int data_length_list[N_ADDRESSES] = {3,
                                                  8,
                                                  1,
                                                  4,
                                                  9,
                                                  3,
                                                  6,
                                                  6,
                                                  3};

    dbgprint("Sending transactions...");
    for (unsigned int i = 0; i < n_transactions; i++) {
      address = address_list[i % N_ADDRESSES];

      data_length = data_length_list[i % N_ADDRESSES];
      unsigned char *data = new unsigned char[data_length];
      //Set data here

      if (address == SOBEL_OUTPUT) {
        dbgprint("Sending READ transaction to address %08x", address);
        tb_initiator->read(data, address, data_length);
      }
      else if (address == IMG_FILTER_KERNEL || address == SOBEL_INPUT_0 || address == SOBEL_INPUT_0) {
        dbgprint("Sending WRITE transaction to address %08x", address);
        tb_initiator->write(data, address, data_length);
      }
      else {
        dbgprint("Sending WRITE transaction to address %08x", address);
        tb_initiator->write(data, address, data_length);
        dbgprint("Sending READ transaction to address %08x", address);
        tb_initiator->read(data, address, data_length);
      }
      wait(tb_send_delay);
    }
  }
};

int sc_main(int, char*[])
{
  // Pass command linke arguments
  sc_argc();
  sc_argv();

  Tb_router_top top("top");
  
  sc_start();

  dbgprint("Terminating simulation");

  return 0;
}

#endif // TB_ROUTER_TLM_CPP
#endif // USING_TLM_ROUTER_TB_EN
