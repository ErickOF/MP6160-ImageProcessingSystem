#ifndef IPS_FILTER_PV_MODEL_HPP
#define IPS_FILTER_PV_MODEL_HPP
#ifdef IPS_DEBUG_EN
#include <iostream>
#endif // IPS_DEBUG_ENi
#ifdef IPS_DUMP_EN
#include <sstream>
#endif // IPS_DUMP_EN
#include <systemc.h>


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
  //-----------------------------Local Variables-----------------------------
#ifdef IPS_DUMP_EN
  sc_trace_file* wf;
#endif // IPS_DUMP_EN
  OUT* kernel;

  /**
   * @brief Default constructor for Filter
   */
  SC_CTOR(Filter);
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
    SC_METHOD(init_kernel);
  }

  //---------------------------------Methods---------------------------------
  void filter(IN* img_window, OUT& result);
  void init_kernel();
};

/**
 * @brief Filtering image
 * 
 * @param img_window - image window to filter
 * @param result - resultant pixel
 */
template <typename IN, typename OUT, uint8_t N>
void Filter<IN, OUT, N>::filter(IN* img_window, OUT& result)
{
  size_t i;
  size_t j;

  // Default value for the result depending on the output datatype
  result = static_cast<OUT >(0);

  // Perform the convolution
  for (i = 0; i < N; ++i)
    for (j = 0; j < N; ++j)
      result += this->kernel[i * N + j] * static_cast<OUT >(img_window[i * N + j]);
}

/**
 * @brief Initializes a kernel of N x N with default value of 1 / (N^2)
 * 
 */
template <typename IN, typename OUT, uint8_t N>
void Filter<IN, OUT, N>::init_kernel()
{
  // Init a kernel of N x N with default value of 1 / (N * N)
  this->kernel = new OUT[N * N];
  std::fill_n(this->kernel, N * N, static_cast<OUT >(1) / static_cast<OUT >(N * N));
#ifdef IPS_DEBUG_EN
  // Print the initialized kernel
  SC_REPORT_INFO(this->name(), "init_kernel result");
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
#endif // IPS_FILTER_PV_MODEL_HPP
