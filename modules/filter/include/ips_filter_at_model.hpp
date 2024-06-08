#ifndef IPS_FILTER_AT_MODEL_HPP
#define IPS_FILTER_AT_MODEL_HPP
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
protected:
  //----------------------------Internal Variables----------------------------
#ifdef IPS_DUMP_EN
  sc_trace_file* wf;
#endif // IPS_DUMP_EN
  OUT* kernel;

  // Event to trigger the filter execution
  sc_event event;

  //-----------------------------Internal Methods-----------------------------
  void exec_filter();
  void init();

public:
  sc_in<IN* > img_window;
  sc_out<OUT > result;
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
  void filter();
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

  OUT result_tmp;

  while (true)
  {
    // Wait to peform the convolution
    wait(this->event);

    // Default value for the result depending on the output datatype
    result_tmp = (OUT) 0;

    // Getting the image window to filter
    IN* img_window_tmp = this->img_window.read();

#ifdef IPS_DEBUG_EN
    std::cout << "Performing convolution" << std::endl;
#endif // IPS_DEBUG_EN

    // Perform the convolution
    for (i = 0; i < N; ++i)
      for (j = 0; j < N; ++j)
      {
#ifdef IPS_DEBUG_EN
        std::cout << "Starting [" << i << "][" << j << "]" << std::endl;
#endif // IPS_DEBUG_EN
        result_tmp += this->kernel[i * N + j] * ((OUT) img_window_tmp[i * N + j]);
#ifdef IPS_DEBUG_EN
        std::cout << "Done [" << i << "][" << j << "]" << std::endl;
#endif // IPS_DEBUG_EN
      }

#ifdef IPS_DEBUG_EN
    std::cout << "Convolution result is done" << std::endl;
#endif // IPS_DEBUG_EN
    this->result.write(result_tmp);
  }
}

/**
 * @brief Filtering image
 * 
 * @param img_window - image window to filter
 * @param result - resultant pixel
 */
template <typename IN, typename OUT, uint8_t N>
void Filter<IN, OUT, N>::filter()
{
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
  std::fill_n(this->kernel, N * N, ((OUT) 1) / ((OUT) N * N));

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
#endif // IPS_DEBUG_EN
}
#endif // IPS_FILTER_AT_MODEL_HPP
