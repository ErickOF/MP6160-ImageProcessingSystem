#ifndef USING_TLM_TB_EN
#define int64  systemc_int64
#define uint64 systemc_uint64
#include <systemc.h>
#ifdef TEST_MODE_ONE_WINDOW_RANDOM
#include <cstdlib>
#elif defined(TEST_MODE_IMAGE)
#include <cstdlib>
#undef int64
#undef uint64
#define int64  opencv_int64
#define uint64 opencv_uint64
#include <opencv2/opencv.hpp>
#undef int64
#undef uint64
#define SYSTEMC_TYPES_ALREADY_DEFINED
#endif // TEST_MODE
#ifdef IPS_DUMP_EN
#include <sstream>
#endif // IPS_DUMP_EN

#ifndef IPS_FILTER_KERNEL_SIZE
#define IPS_FILTER_KERNEL_SIZE 3
#endif // IPS_FILTER_KERNEL_SIZE
#ifndef IPS_IN_TYPE_TB
#define IPS_IN_TYPE_TB float
#endif // IPS_IN_TYPE_TB
#ifndef IPS_OUT_TYPE_TB
#define IPS_OUT_TYPE_TB float
#endif // IPS_OUT_TYPE_TB
#ifndef IPS_IMG_PATH_TB
#define IPS_IMG_PATH_TB "../../tools/datagen/src/imgs/car_grayscale_image.jpg"
#endif // IPS_IMG_PATH_TB

#ifndef IPS_FILTER_PV_EN
// N * N * copy_pixel_to_mem_time + mult + redux + copy_pixel_to_mem_time
// Image is copied pixel by pixel
#define DELAY_TIME (IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE * 1) + 4 + 2 + 1
#endif // IPS_FILTER_PV_EN

#ifdef IPS_FILTER_AT_EN
#include "ips_filter_at_model.hpp"
#elif defined(IPS_FILTER_LT_EN)
#include "ips_filter_lt_model.hpp"
#elif defined(IPS_FILTER_PV_EN)
#include "ips_filter_pv_model.hpp"
#else
#error "No IPS_FILTER macro is defined. Please define one of IPS_FILTER_AT_EN, IPS_FILTER_LT_EN, or IPS_FILTER_PV_EN."
#endif // IPS_FILTER_XX_EN


#ifdef TEST_MODE_IMAGE
#ifdef IPS_DUMP_EN
void run_image(sc_trace_file* wf)
#else
void run_image()
#endif // IPS_DUMP_EN
{
#ifdef IPS_DEBUG_EN
  SC_REPORT_INFO("TEST_MODE_IMAGE", "Running test");
#endif // IPS_DEBUG_EN

  const std::string img_path = IPS_IMG_PATH_TB;
  cv::Mat read_image = cv::imread(img_path, cv::IMREAD_GRAYSCALE);
  cv::Mat image;
  read_image.convertTo(image, CV_32F);

  cv::Mat o_img(image.size(), image.type());

#ifdef IPS_DUMP_EN
  std::cout << "Loading image: " << img_path << std::endl;
#endif // IPS_DUMP_EN

  // Check if the image is loaded successfully
  if (image.empty())
  {
    std::cerr << "Error: Could not open or find the image!" << std::endl;
    exit(EXIT_FAILURE);
  }

#ifdef IPS_DUMP_EN
  std::cout << "Image info: ";
  std::cout << "rows = " << image.rows;
  std::cout << " cols = " << image.cols;
  std::cout << " channels = " << image.channels() << std::endl;
#endif // IPS_DUMP_EN

  // Variables
  IPS_IN_TYPE_TB* img_window;
  IPS_OUT_TYPE_TB result;

  int i, j;
  int x, y;

#ifdef IPS_FILTER_AT_EN
  sc_signal<IPS_IN_TYPE_TB*> s_img_window;
  sc_signal<IPS_OUT_TYPE_TB> s_result;
#endif // IPS_FILTER_AT_EN

  // Initialize image window
  img_window = new IPS_IN_TYPE_TB[IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE];

  // Instantiate filter module and do the connection
#ifdef IPS_DUMP_EN
  for (i = 0; i < IPS_FILTER_KERNEL_SIZE; ++i)
  {
    for (j = 0; j < IPS_FILTER_KERNEL_SIZE; ++j)
    {
      std::ostringstream var_name;
      var_name << "img_window_" << i << "_" << j;
      sc_trace(wf, img_window[i * IPS_FILTER_KERNEL_SIZE + j], var_name.str());
    }
  }

  Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE> filter("filter", wf);
  sc_trace(wf, result, "result");
#else
  Filter<IPS_IN_TYPE_TB, IPS_OUT_TYPE_TB, IPS_FILTER_KERNEL_SIZE> filter("filter");
#endif // IPS_DUMP_EN
#ifdef IPS_FILTER_AT_EN
  filter.img_window(s_img_window);
  filter.result(s_result);
#endif // IPS_FILTER_AT_EN

  sc_start();

#ifdef IPS_DEBUG_EN
  std::cout << "Test starting" << std::endl;
  std::cout << "@" << sc_time_stamp() << std::endl;
#endif // IPS_DEBUG_EN

  // Create each window
  for (y = 0; y < image.rows - IPS_FILTER_KERNEL_SIZE; ++y)
  {
    for (x = 0; x < image.cols - IPS_FILTER_KERNEL_SIZE; ++x)
    {
#ifdef IPS_DEBUG_EN
      SC_REPORT_INFO("TEST_MODE_IMAGE", "filtering");
#endif // IPS_DEBUG_EN

      // Define the ROI
      cv::Rect roi(x, y, IPS_FILTER_KERNEL_SIZE, IPS_FILTER_KERNEL_SIZE);
      cv::Mat sub_img = image(roi);

      for (i = 0; i < IPS_FILTER_KERNEL_SIZE; ++i)
      {
        for (j = 0; j < IPS_FILTER_KERNEL_SIZE; ++j)
        {
          img_window[i * IPS_FILTER_KERNEL_SIZE + j] = sub_img.at<IPS_IN_TYPE_TB>(i, j);
#ifdef IPS_DEBUG_EN
          std::cout << "[" << img_window[i * IPS_FILTER_KERNEL_SIZE + j] << "]";
#endif // IPS_DEBUG_EN
        }

#ifdef IPS_DEBUG_EN
        std::cout << std::endl;
#endif // IPS_DEBUG_EN
      }

      // Apply convolution
#ifdef IPS_FILTER_PV_EN
      filter.filter(img_window, result);
#elif defined(IPS_FILTER_LT_EN)
      filter.filter(img_window, &result);
      sc_start(DELAY_TIME + 10, SC_NS);
#elif defined(IPS_FILTER_AT_EN)
      s_img_window.write(img_window);
      filter.filter();
      sc_start(DELAY_TIME + 10, SC_NS);

      result = s_result.read();
#endif // IPS_FILTER_XX_EN

      o_img.at<IPS_OUT_TYPE_TB>(y, x) = result;

#ifdef IPS_DEBUG_EN
      std::cout << "Result[" << x << "][" << y << "] = " << o_img.at<IPS_OUT_TYPE_TB>(y, x) << std::endl << std::endl;
#endif // IPS_DEBUG_EN
    }
  }

#ifdef IPS_DUMP_EN
  sc_start(1, SC_NS);
#endif // IPS_DUMP_EN

  // Convert the floating-point image to 8-bit unsigned integer for saving
  cv::Mat final_img;
  o_img.convertTo(final_img, CV_8U, 1.0);

  // Save the final image
  std::string output_img_path = "filtered_image.png";
  cv::imwrite(output_img_path, final_img);

  delete [] img_window;
}
#endif // TEST_MODE_IMAGE

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
#endif // IPS_DEBUG_EN

  // Variables
  IPS_IN_TYPE_TB* img_window;
  IPS_OUT_TYPE_TB result;

#ifdef IPS_FILTER_AT_EN
  sc_signal<IPS_IN_TYPE_TB*> s_img_window;
  sc_signal<IPS_OUT_TYPE_TB> s_result;
#endif // IPS_FILTER_AT_EN

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
#ifdef IPS_FILTER_AT_EN
  s_img_window.write(img_window);

  filter.img_window(s_img_window);
  filter.result(s_result);
#endif // IPS_FILTER_AT_EN

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
  sc_start(DELAY_TIME + 10, SC_NS);
#elif defined(IPS_FILTER_AT_EN)
  filter.filter();
  sc_start(DELAY_TIME + 10, SC_NS);

  result = s_result.read();
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
#ifdef IPS_DUMP_EN
  run_one_window(wf);
#else
  run_one_window();
#endif // IPS_DUMP_EN
#elif defined(TEST_MODE_IMAGE)
#ifdef IPS_DUMP_EN
  run_image(wf);
#else
  run_image();
#endif // IPS_DUMP_EN
#endif // TEST_MODE

#ifdef IPS_DUMP_EN
  std::cout << "@" << sc_time_stamp() << " Terminating simulation" << std::endl;
  sc_close_vcd_trace_file(wf);
#endif // IPS_DUMP_EN
  return 0;
}

#endif // USING_TLM_TB_EN
