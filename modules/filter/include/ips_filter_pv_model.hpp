#ifndef IPS_FILTER_FILTER_HPP
#define IPS_FILTER_FILTER_HPP
#ifdef IPS_DEBUG_EN
#include <iostream>
#endif // IPS_DEBUG_ENi
#ifdef IPS_DUMP_EN
#include <sstream>
#endif // IPS_DUMP_EN
#include <systemc.h>
#include <vector>


template <typename IN = sc_uint<8>, typename OUT = sc_uint<8>, uint8_t N = 3>
SC_MODULE(Filter)
{
  //-----------------------------Local Variables-----------------------------
#ifdef IPS_DUMP_EN
  sc_trace_file* wf;
#endif // IPS_DUMP_EN
  OUT* kernel;

  /**
   * @brief Default constructor for Filter
   */
  SC_CTOR(Filter);
  //template <typename IN, typename OUT, uint8_t N>
#ifdef IPS_DUMP_EN
  Filter(sc_core::sc_module_name name, sc_core::sc_trace_file* wf)
    : sc_core::sc_module(name), wf(wf)
#else
  Filter(sc_core::sc_module_name name) : sc_core::sc_module(name)
#endif // IPS_DUMP_EN
  {
    SC_METHOD(init_kernel);
  }

  //---------------------------------Methods---------------------------------
  void filter(IN* img_window, OUT& result);
  void init_kernel();
};

template <typename IN, typename OUT, uint8_t N>
void Filter<IN, OUT, N>::filter(IN* img_window, OUT& result)
{
  size_t i;
  size_t j;

  result = (OUT) 0;

  for (i = 0; i < N; ++i)
    for (j = 0; j < N; ++j)
      result += this->kernel[i * N + j] * ((OUT) img_window[i * N + j]);
}

template <typename IN, typename OUT, uint8_t N>
void Filter<IN, OUT, N>::init_kernel()
{
  // Init a kernel of N x N with default value of 1 / (N * N)
  this->kernel = new OUT[N * N];
  std::fill_n(this->kernel, N * N, ((OUT) 1) / ((OUT) N * N));
#ifdef IPS_DEBUG_EN
  SC_REPORT_INFO(this->name(), "init_kernel result");
  size_t i, j;

  for (i = 0; i < N; ++i)
  {
    for (j = 0; j < N; ++j)
    {
      std::cout << "[" << this->kernel[i * N + j] << "]";

#ifdef IPS_DUMP_EN
      std::ostringstream var_name;
      var_name << "kernel_" << i << "_" << j;
      sc_trace(this->wf, this->kernel[i * N + j], var_name.str());
#endif // IPS_DUMP_EN
    }

    std::cout << std::endl;
  }
#endif // IPS_DEBUG_EN
}
#endif // IPS_FILTER_FILTER_HPP
