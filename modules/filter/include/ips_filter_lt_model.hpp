#ifndef IPS_FILTER_LT_MODEL_HPP
#define IPS_FILTER_LT_MODEL_HPP
#ifdef IPS_DEBUG_EN
#include <iostream>
#endif // IPS_DEBUG_ENi
#ifdef IPS_DUMP_EN
#include <sstream>
#endif // IPS_DUMP_EN
#include <systemc.h>

#ifdef USING_TLM_TB_EN
#include "ips_filter_defines.hpp"
#endif // USING_TLM_TB_EN

/**
 * @brief Filter module.
 * It takes care of filtering a image/kernel using a median filter or an
 * equivalent convolution like:
 *                 | 1/N^2 ... 1/N^2 |   | img(row - N/2, col - N/2) ... img(row + N/2, col + N/2) |
 * img(row, col) = |  ...  ...  .... | * |            ...            ...            ...            |
 *                 | 1/N^2 ... 1/N^2 |   | img(row + N/2, col - N/2) ... img(row + N/2, col + N/2) |
 *
 * @tparam IN - data type of the inputs
 * @tparam OUT - data type of the outputs
 * @tparam N - size of the kernel
 */
template <typename IN = sc_uint<8>, typename OUT = sc_uint<8>, uint8_t N = 3>
SC_MODULE(Filter)
{
protected:
  //----------------------------Internal Variables----------------------------
#ifdef IPS_DUMP_EN
  sc_trace_file* wf;
#endif // IPS_DUMP_EN
  OUT* img_window_tmp;
  OUT* kernel;
  OUT* result_ptr;

  // Event to trigger the filter execution
  sc_event event;

  //-----------------------------Internal Methods-----------------------------
  void exec_filter();
  void init();

public:
  /**
   * @brief Default constructor for Filter
   */
  SC_HAS_PROCESS(Filter);
#ifdef IPS_DUMP_EN
  /**
   * @brief Construct a new Filter object
   * 
   * @param name - name of the module
   * @param wf - waveform file pointer
   */
  Filter(sc_core::sc_module_name name, sc_core::sc_trace_file* wf)
    : sc_core::sc_module(name), wf(wf)
#else
  /**
   * @brief Construct a new Filter object
   * 
   * @param name - name of the module
   */
  Filter(sc_core::sc_module_name name) : sc_core::sc_module(name)
#endif // IPS_DUMP_EN
  {
    // Calling this method by default since it is no time consumer
    // It is assumed that this kernel is already loaded in the model
    // Kernel does not change after synthesis
    SC_METHOD(init);
    // Thread waiting for the request
    SC_THREAD(exec_filter);
  }

  //---------------------------------Methods---------------------------------
  void filter(IN* img_window, OUT* result);
};

/**
 * @brief Execute the image filtering
 * 
 */
template <typename IN, typename OUT, uint8_t N>
void Filter<IN, OUT, N>::exec_filter()
{
  size_t i;
  size_t j;

  while (true)
  {
    // Wait to peform the convolution
    wait(this->event);

    // Default value for the result depending on the output datatype
    *(this->result_ptr) = static_cast<OUT >(0);

    // Perform the convolution
    for (i = 0; i < N; ++i)
      for (j = 0; j < N; ++j)
        *(this->result_ptr) += this->kernel[i * N + j] * this->img_window_tmp[i * N + j];
  }
}

/**
 * @brief Filtering image
 * 
 * @param img_window - image window to filter
 * @param result - resultant pixel
 */
template <typename IN, typename OUT, uint8_t N>
void Filter<IN, OUT, N>::filter(IN* img_window, OUT* result)
{
  size_t i;
  size_t j;

  // Default value for the result depending on the output datatype
  this->result_ptr = result;

  // Perform the convolution
  for (i = 0; i < N; ++i)
    for (j = 0; j < N; ++j)
        this->img_window_tmp[i * N + j] = static_cast<OUT >(img_window[i * N + j]);

  this->event.notify(DELAY_TIME, SC_NS);
}

/**
 * @brief Initializes a kernel of N x N with default value of 1 / (N^2)
 * 
 */
template <typename IN, typename OUT, uint8_t N>
void Filter<IN, OUT, N>::init()
{
  // Init a kernel of N x N with default value of 1 / (N * N)
  this->kernel = new OUT[N * N];
  std::fill_n(this->kernel, N * N, static_cast<OUT >(1) / static_cast<OUT > (N * N));
  // Init image window of N x N with default value of 1 / (N * N)
  this->img_window_tmp = new OUT[N * N];
#ifdef IPS_DEBUG_EN
  // Print the initialized kernel
  SC_REPORT_INFO(this->name(), "init result");
  size_t i, j;

  for (i = 0; i < N; ++i)
  {
    for (j = 0; j < N; ++j)
    {
      std::cout << "[" << this->kernel[i * N + j] << "]";

#ifdef IPS_DUMP_EN
      // Adding the signals to the waveform
      std::ostringstream var_name;
      var_name << "kernel_" << i << "_" << j;
      sc_trace(this->wf, this->kernel[i * N + j], var_name.str());
#endif // IPS_DUMP_EN
    }

    std::cout << std::endl;
  }
#else
#ifdef IPS_DUMP_EN
  size_t i, j;

  for (i = 0; i < N; ++i)
  {
    for (j = 0; j < N; ++j)
    {
      // Adding the signals to the waveform
      std::ostringstream var_name;
      var_name << "kernel_" << i << "_" << j;
      sc_trace(this->wf, this->kernel[i * N + j], var_name.str());
    }
  }
#endif // IPS_DUMP_EN
#endif // IPS_DEBUG_EN
}
#endif // IPS_FILTER_LT_MODEL_HPP
