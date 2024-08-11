
#define int64  systemc_int64
#define uint64 systemc_uint64
#include <systemc.h>

#include <cstdlib>

#undef int64
#undef uint64

#define int64  opencv_int64
#define uint64 opencv_uint64
#include <opencv2/opencv.hpp>
#undef int64
#undef uint64

#ifdef IPS_DUMP_EN
#include <sstream>
#endif // IPS_DUMP_EN

#define IPS_FILTER_KERNEL_SIZE 3
#define IPS_IMG_PATH_TB "../../tools/datagen/src/imgs/car_grayscale_image.jpg"

#define DELAY_TIME (IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE * 1) + 4 + 2 + 1

#ifdef __RTL_SIMULATION__
#include "Filter_rtl_wrapper.h"
#define Filter Filter_rtl_wrapper
#else
#include "ips_filter.hpp"
#endif //__RTL_SIMULATION__

#include "tb_module.cpp"

//#ifdef TEST_MODE_IMAGE
//#ifdef IPS_DUMP_EN
//void run_image(sc_trace_file* wf)
//#else
//void run_image()
//#endif // IPS_DUMP_EN
//{
//#ifdef IPS_DEBUG_EN
//  SC_REPORT_INFO("TEST_MODE_IMAGE", "Running test");
//#endif // IPS_DEBUG_EN
//
//  const std::string img_path = IPS_IMG_PATH_TB;
//  cv::Mat read_image = cv::imread(img_path, cv::IMREAD_GRAYSCALE);
//  cv::Mat image;
//  read_image.convertTo(image, CV_32F);
//
//  cv::Mat o_img(image.size(), image.type());
//
//#ifdef IPS_DUMP_EN
//  std::cout << "Loading image: " << img_path << std::endl;
//#endif // IPS_DUMP_EN
//
//  // Check if the image is loaded successfully
//  if (image.empty())
//  {
//    std::cerr << "Error: Could not open or find the image!" << std::endl;
//    exit(EXIT_FAILURE);
//  }
//
//#ifdef IPS_DUMP_EN
//  std::cout << "Image info: ";
//  std::cout << "rows = " << image.rows;
//  std::cout << " cols = " << image.cols;
//  std::cout << " channels = " << image.channels() << std::endl;
//#endif // IPS_DUMP_EN
//
//  // Variables
//  sc_uint<8>* img_window;
//  sc_uint<8> result;
//
//  int i, j;
//  int x, y;
//
//  sc_signal<sc_uint<8>> s_img_window[IPS_FILTER_KERNEL_SIZE*IPS_FILTER_KERNEL_SIZE];
//  sc_signal<sc_uint<8>> s_result;
//
//  // Initialize image window
//  img_window = new sc_uint<8>[IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE];
//
//  // Instantiate filter module and do the connection
//#ifdef IPS_DUMP_EN
//  Filter filter("filter", wf);
//  sc_trace(wf, result, "result");
//#else
//  Filter filter("filter");
//#endif // IPS_DEBUG_EN
//
//  filter.input_window(s_img_window);
//  filter.output(s_result);
//
//  // Instantiate filter module and do the connection
//#ifdef IPS_DUMP_EN
//  for (i = 0; i < IPS_FILTER_KERNEL_SIZE; ++i)
//  {
//    for (j = 0; j < IPS_FILTER_KERNEL_SIZE; ++j)
//    {
//      std::ostringstream var_name;
//      var_name << "img_window_" << i << "_" << j;
//      sc_trace(wf, img_window[i * IPS_FILTER_KERNEL_SIZE + j], var_name.str());
//    }
//  }
//
//  sc_start();
//
//#ifdef IPS_DEBUG_EN
//  std::cout << "Test starting" << std::endl;
//  std::cout << "@" << sc_time_stamp() << std::endl;
//#endif // IPS_DEBUG_EN
//
//  // Create each window
//  for (y = 0; y < image.rows - IPS_FILTER_KERNEL_SIZE; ++y)
//  {
//    for (x = 0; x < image.cols - IPS_FILTER_KERNEL_SIZE; ++x)
//    {
//#ifdef IPS_DEBUG_EN
//      SC_REPORT_INFO("TEST_MODE_IMAGE", "filtering");
//#endif // IPS_DEBUG_EN
//
//      // Define the ROI
//      cv::Rect roi(x, y, IPS_FILTER_KERNEL_SIZE, IPS_FILTER_KERNEL_SIZE);
//      cv::Mat sub_img = image(roi);
//
//      for (i = 0; i < IPS_FILTER_KERNEL_SIZE; ++i)
//      {
//        for (j = 0; j < IPS_FILTER_KERNEL_SIZE; ++j)
//        {
//          img_window[i * IPS_FILTER_KERNEL_SIZE + j] = sub_img.at<sc_uint<8>>(i, j);
//#ifdef IPS_DEBUG_EN
//          std::cout << "[" << img_window[i * IPS_FILTER_KERNEL_SIZE + j] << "]";
//#endif // IPS_DEBUG_EN
//        }
//
//#ifdef IPS_DEBUG_EN
//        std::cout << std::endl;
//#endif // IPS_DEBUG_EN
//      }
//
//      // Apply convolution
//      s_img_window.write(img_window);
//      filter.filter();
//      sc_start(DELAY_TIME + 10, SC_NS);
//
//      result = s_result.read();
//
//      o_img.at<sc_uint<8>>(y, x) = result;
//
//#ifdef IPS_DEBUG_EN
//      std::cout << "Result[" << x << "][" << y << "] = " << o_img.at<sc_uint<8>>(y, x) << std::endl << std::endl;
//#endif // IPS_DEBUG_EN
//    }
//  }
//
//#ifdef IPS_DUMP_EN
//  sc_start(1, SC_NS);
//#endif // IPS_DUMP_EN
//
//  // Convert the floating-point image to 8-bit unsigned integer for saving
//  cv::Mat final_img;
//  o_img.convertTo(final_img, CV_8U, 1.0);
//
//  // Save the final image
//  std::string output_img_path = "filtered_image.png";
//  cv::imwrite(output_img_path, final_img);
//
//  delete [] img_window;
//}
//#endif // TEST_MODE_IMAGE
//
//#ifdef TEST_MODE_ONE_WINDOW
//#ifdef IPS_DUMP_EN
//void run_one_window(sc_trace_file* wf)
//#else
//void run_one_window()
//#endif // IPS_DUMP_EN
//{
//#ifdef IPS_DEBUG_EN
//  #ifdef TEST_MODE_ONE_WINDOW_RANDOM
//  SC_REPORT_INFO("TEST_MODE_ONE_WINDOW_NORMAL", "Running test");
//  #elif defined(TEST_MODE_ONE_WINDOW_NORMAL)
//  SC_REPORT_INFO("TEST_MODE_ONE_WINDOW_NORMAL", "Running test");
//  #else
//  SC_REPORT_INFO("TEST_MODE_ONE_WINDOW_DEFAULT", "Running test");
//  #endif // TEST_MODE_ONE_WINDOW_RANDOM
//  SC_REPORT_INFO("Initialize window", "Window value");
//#endif // IPS_DEBUG_EN
//
//  // Variables
//  sc_uint<8>* img_window;
//  sc_uint<8> result;
//
//  sc_signal<sc_uint<8>> s_img_window;
//  sc_signal<sc_uint<8>> s_result;
//
//  // Instantiate filter module and do the connection
//#ifdef IPS_DUMP_EN
//  Filter filter("filter", wf);
//  sc_trace(wf, result, "result");
//#else
// Filter filter("filter");
//#endif // IPS_DEBUG_EN
//
// filter.input_window[0](s_img_window);
// filter.input_window[1](s_img_window);
// filter.input_window[2](s_img_window);
// filter.input_window[3](s_img_window);
// filter.input_window[4](s_img_window);
// filter.input_window[5](s_img_window);
// filter.input_window[6](s_img_window);
// filter.input_window[7](s_img_window);
// filter.input_window[8](s_img_window);
// filter.output(s_result);
//
//  sc_start();
//#ifdef IPS_DEBUG_EN
//  std::cout << "Test starting" << std::endl;
//  std::cout << "@" << sc_time_stamp() << std::endl;
//#endif // IPS_DEBUG_EN
//
//  // Initialize image window
//  img_window = new sc_uint<8>[IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE];
//
//    for (size_t i = 0; i < IPS_FILTER_KERNEL_SIZE; ++i)
//    {
//      for (size_t j = 0; j < IPS_FILTER_KERNEL_SIZE; ++j)
//      {
//        sc_uint<8> value;
//
//  #ifdef TEST_MODE_ONE_WINDOW_RANDOM
//        value = (sc_uint<8>) (rand() % 256);
//  #elif defined(TEST_MODE_ONE_WINDOW_NORMAL)
//        value = (sc_uint<8>) (i * IPS_FILTER_KERNEL_SIZE + j);
//  #else
//        value = (sc_uint<8>) i;
//  #endif // TEST_MODE_ONE_WINDOW
//
//        img_window[i * IPS_FILTER_KERNEL_SIZE + j] = value;
//
//  #ifdef IPS_DUMP_EN
//        std::ostringstream var_name;
//        var_name << "img_window_" << i << "_" << j;
//        sc_trace(wf, img_window[i * IPS_FILTER_KERNEL_SIZE + j], var_name.str());
//  #endif // IPS_DUMP_EN
//  #ifdef IPS_DEBUG_EN
//        std::cout << "[" << (int) img_window[i * IPS_FILTER_KERNEL_SIZE + j] << "]";
//  #endif // IPS_DUMP_EN
//      }
//
//  #ifdef IPS_DEBUG_EN
//      std::cout << std::endl;
//  #endif // IPS_DEBUG_EN
//    }
//
//    for (int i = 0; i < IPS_FILTER_KERNEL_SIZE*IPS_FILTER_KERNEL_SIZE; i++) {
//#ifdef IPS_DEBUG_EN
//	SC_REPORT_INFO("[DEBUG]", "Writing to image window value:");
//	std::cout << "Iteration: " << i << " img_window = " << (int) img_window[i] << std::endl;
//#endif // IPS_DEBUG_EN
//  	  s_img_window.write(img_window[i]);
//      sc_uint<8> value2 = s_img_window.read();
//
//#ifdef IPS_DEBUG_EN
//	std::cout << "Iteration: " << i << " s_img_window = " << (int) value2 << std::endl;
//#endif // IPS_DEBUG_EN
//    }
//
//  // Apply convolution
//  filter.filter();
//  sc_start(DELAY_TIME + 10, SC_NS);
//
//  result = s_result.read();
//
//#ifdef IPS_DEBUG_EN
//  SC_REPORT_INFO("TEST_MODE_ONE_WINDOW", "filtering");
//  std::cout << "Result = " << result << std::endl;
//#endif // IPS_DEBUG_EN
//
//#ifdef IPS_DUMP_EN
//  sc_start(1, SC_NS);
//#endif // IPS_DUMP_EN
//
//  delete [] img_window;
//}
//#endif // TEST_MODE_ONE_WINDOW


int sc_main(int, char*[])
{
	// sc_report_handler::set_actions( "/IEEE_Std_1666/deprecated" , SC_DO_NOTHING);
	// sc_report_handler::set_actions( SC_ID_LOGIC_X_TO_BOOL_, SC_LOG);
	// sc_report_handler::set_actions( SC_ID_VECTOR_CONTAINS_LOGIC_VALUE_, SC_LOG);
	// sc_report_handler::set_actions( SC_ID_OBJECT_EXISTS_, SC_LOG);
	// Pass command linke arguments
	sc_argc();
	sc_argv();

	sc_signal<sc_uint<8> > signal;
  sc_signal<sc_uint<8> > signal_bus[9];
	sc_clock clk("clk", 1, SC_NS);

	testbench tb_module("tb_module");
	Filter filter("filter");

	filter.clk(clk);
	filter.input_window_0(signal_bus[0]);
	filter.input_window_1(signal_bus[1]);
	filter.input_window_2(signal_bus[2]);
	filter.input_window_3(signal_bus[3]);
	filter.input_window_4(signal_bus[4]);
	filter.input_window_5(signal_bus[5]);
	filter.input_window_6(signal_bus[6]);
	filter.input_window_7(signal_bus[7]);
	filter.input_window_8(signal_bus[8]);

	for (int i = 0; i < 9; i++){
		tb_module.img_window[i](signal_bus[i]);
	}
	filter.output(signal);
	tb_module.mean(signal);

	sc_start();
#ifdef IPS_DEBUG_EN
	std::cout << "Test starting" << std::endl;
	std::cout << "@" << sc_time_stamp() << std::endl;
#endif // IPS_DEBUG_EN


//  printf(" Signal is %0u\n", signal.read());
//  std::cout << "Signal is " << (int) signal.read() << std::endl;
//  sc_start();
//  printf(" Signal is %0u\n", signal.read());
//  std::cout << "Signal is " << (int) signal.read() << std::endl;

// #ifdef IPS_DUMP_EN
//   // Open VCD file
// #endif // IPS_DUMP_EN

// #ifdef TEST_MODE_ONE_WINDOW
// #ifdef IPS_DUMP_EN
//   run_one_window(wf);
// #else
//   run_one_window();
// #endif // IPS_DUMP_EN
// #elif defined(TEST_MODE_IMAGE)
// #ifdef IPS_DUMP_EN
//   run_image(wf);
// #else
//   run_image();
// #endif // IPS_DUMP_EN
// #endif // TEST_MODE

// #ifdef IPS_DUMP_EN
//   std::cout << "@" << sc_time_stamp() << " Terminating simulation" << std::endl;
//   sc_close_vcd_trace_file(wf);
// #endif // IPS_DUMP_EN
  return 0;
}

