#ifdef TEST_MODE_ONE_WINDOW_RANDOM
#include <cstdlib>
#endif // TEST_MODE_ONE_WINDOW_RANDOM
#ifdef IPS_DUMP_EN
#include <sstream>
#endif // IPS_DUMP_EN
#include <systemc.h>

#ifndef IPS_FILTER_KERNEL_SIZE
#define IPS_FILTER_KERNEL_SIZE 3
#endif // IPS_FILTER_KERNEL_SIZE
#ifndef IPS_IN_TYPE_TB
#define IPS_IN_TYPE_TB float
#endif // IPS_IN_TYPE_TB
#ifndef IPS_OUT_TYPE_TB
#define IPS_OUT_TYPE_TB float
#endif // IPS_OUT_TYPE_TB

#ifdef IPS_FILTER_AT_EN
#include "ips_filter_at_model.hpp"
#elif defined(IPS_FILTER_LT_EN)
#include "ips_filter_lt_model.hpp"
#elif defined(IPS_FILTER_PV_EN)
#include "ips_filter_pv_model.hpp"
#else
#error "No IPS_FILTER macro is defined. Please define one of IPS_FILTER_AT_EN, IPS_FILTER_LT_EN, or IPS_FILTER_PV_EN."
#endif // IPS_FILTER_XX_EN


#ifdef TEST_MODE_ONE_WINDOW
#ifdef IPS_DUMP_EN
void run_one_window(sc_trace_file* wf)
#else
void run_one_window()
#endif // IPS_DUMP_EN
{
#ifdef IPS_DEBUG_EN
  #ifdef TEST_MODE_ONE_WINDOW_RANDOM
  SC_REPORT_INFO("TEST_MODE_ONE_WINDOW_NORMAL", "Running test");
  #elif defined(TEST_MODE_ONE_WINDOW_NORMAL)
  SC_REPORT_INFO("TEST_MODE_ONE_WINDOW_NORMAL", "Running test");
  #else
  SC_REPORT_INFO("TEST_MODE_ONE_WINDOW_DEFAULT", "Running test");
  #endif // TEST_MODE_ONE_WINDOW_RANDOM
  SC_REPORT_INFO("Initialize window", "Window value");
#endif

  // Variables
  IPS_IN_TYPE_TB *img_window;
  IPS_OUT_TYPE_TB result;

  // Initialize image window
  img_window = new IPS_IN_TYPE_TB[IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE];

  for (size_t i = 0; i < IPS_FILTER_KERNEL_SIZE; ++i)
  {
    for (size_t j = 0; j < IPS_FILTER_KERNEL_SIZE; ++j)
    {
      IPS_IN_TYPE_TB value;

#ifdef TEST_MODE_ONE_WINDOW_RANDOM
      value = (IPS_IN_TYPE_TB) (rand() % 256);
#elif defined(TEST_MODE_ONE_WINDOW_NORMAL)
      value = (IPS_IN_TYPE_TB) (i * IPS_FILTER_KERNEL_SIZE + j);
#else
      value = (IPS_IN_TYPE_TB) i;
#endif // TEST_MODE_ONE_WINDOW

      img_window[i * IPS_FILTER_KERNEL_SIZE + j] = value;

#ifdef IPS_DUMP_EN
      std::ostringstream var_name;
      var_name << "img_window_" << i << "_" << j;
      sc_trace(wf, img_window[i * IPS_FILTER_KERNEL_SIZE + j], var_name.str());
#endif // IPS_DUMP_EN
#ifdef IPS_DEBUG_EN
      std::cout << "[" << img_window[i * IPS_FILTER_KERNEL_SIZE + j] << "]";
#endif // IPS_DUMP_EN
    }

#ifdef IPS_DEBUG_EN
    std::cout << std::endl;
#endif // IPS_DEBUG_EN
  }

  // Instantiate filter module and do the connection
#ifdef IPS_DUMP_EN
  Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE> filter("filter", wf);
  sc_trace(wf, result, "result");
#else
  Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE> filter("filter");
#endif // IPS_DEBUG_EN

  sc_start();

#ifdef IPS_DEBUG_EN
  std::cout << "Test starting" << std::endl;
  std::cout << "@" << sc_time_stamp() << std::endl;
#endif // IPS_DEBUG_EN

  // Apply convolution
#ifdef IPS_FILTER_PV_EN
  filter.filter(img_window, result);
#elif defined(IPS_FILTER_LT_EN)
  filter.filter(img_window, &result);
  sc_start(100, SC_NS);
#endif // IPS_FILTER_XX_EN

#ifdef IPS_DEBUG_EN
  SC_REPORT_INFO("TEST_MODE_ONE_WINDOW", "filtering");
  std::cout << "Result = " << result << std::endl;
#endif // IPS_DEBUG_EN

#ifdef IPS_DUMP_EN
  sc_start(1, SC_NS);
#endif // IPS_DUMP_EN

  delete [] img_window;
}
#endif // TEST_MODE_ONE_WINDOW


int sc_main(int, char*[])
{
  // Pass command linke arguments
  sc_argc();
  sc_argv();

#ifdef IPS_DUMP_EN
  // Open VCD file
  sc_trace_file* wf = sc_create_vcd_trace_file("ips_filter");
  wf->set_time_unit(1, SC_NS);
#endif // IPS_DUMP_EN

#ifdef TEST_MODE_ONE_WINDOW
  run_one_window(wf);
#endif // TEST_MODE_ONE_WINDOW

#ifdef IPS_DUMP_EN
  std::cout << "@" << sc_time_stamp() << " Terminating simulation" << std::endl;
  sc_close_vcd_trace_file(wf);
#endif // IPS_DUMP_EN
  return 0;
}
