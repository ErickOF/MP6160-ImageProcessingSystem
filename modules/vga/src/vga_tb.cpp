#define int64  systemc_int64
#define uint64 systemc_uint64
#include <systemc.h>

#undef int64
#undef uint64
#define int64  opencv_int64
#define uint64 opencv_uint64
#include <opencv2/opencv.hpp>
#undef int64
#undef uint64

#include "vga.hpp"


// Image path
#define IPS_IMG_PATH_TB "../../tools/datagen/src/imgs/car_rgb_noisy_image.jpg"
// Main clock frequency in Hz - 25.175 MHz
#define CLK_FREQ 25175000
// VGA settings
#define H_ACTIVE 640
#define H_FP 16
#define H_SYNC_PULSE 96
#define H_BP 48
#define V_ACTIVE 480
#define V_FP 10
#define V_SYNC_PULSE 2
#define V_BP 33
// Compute the total number of pixels
#define TOTAL_VERTICAL (H_ACTIVE + H_FP + H_SYNC_PULSE + H_BP)
#define TOTAL_HORIZONTAL (V_ACTIVE + V_FP + V_SYNC_PULSE + V_BP)
#define TOTAL_PIXELES (TOTAL_VERTICAL * TOTAL_HORIZONTAL)
// Number of bits for ADC, DAC and VGA
#define BITS 8


int sc_main(int, char*[])
{
  // Read image
  const std::string img_path = IPS_IMG_PATH_TB;

  cv::Mat read_img = cv::imread(img_path, cv::IMREAD_COLOR);

  // CV_8UC3 Type: 8-bit unsigned, 3 channels (e.g., for a color image)
  cv::Mat tx_img;
  read_img.convertTo(tx_img, CV_8UC3);

  cv::Mat rx_data(TOTAL_HORIZONTAL, TOTAL_VERTICAL, CV_8UC3);
  cv::Mat rx_img(tx_img.size(), CV_8UC3);

#ifdef IPS_DEBUG_EN
  std::cout << "Loading image: " << img_path << std::endl;
#endif // IPS_DEBUG_EN

  // Check if the image is loaded successfully
  if (tx_img.empty())
  {
    std::cerr << "Error: Could not open or find the image!" << std::endl;
    exit(EXIT_FAILURE);
  }

#ifdef IPS_DEBUG_EN
  std::cout << "TX image info: ";
  std::cout << "rows = " << tx_img.rows;
  std::cout << " cols = " << tx_img.cols;
  std::cout << " channels = " << tx_img.channels() << std::endl;

  std::cout << "RX data info: ";
  std::cout << "rows = " << rx_data.rows;
  std::cout << " cols = " << rx_data.cols;
  std::cout << " channels = " << rx_data.channels() << std::endl;

  std::cout << "RX image info: ";
  std::cout << "rows = " << rx_img.rows;
  std::cout << " cols = " << rx_img.cols;
  std::cout << " channels = " << rx_img.channels() << std::endl;
#endif // IPS_DEBUG_EN

  // Compute the clock time in seconds
  const double CLK_TIME = 1.0 / static_cast<double>(CLK_FREQ);
  // Compute the total simulation based on the total amount of pixels in the
  // screen
  const double SIM_TIME = CLK_TIME * static_cast<double>(TOTAL_PIXELES);

  // Signals to use
  // -- Inputs
  sc_core::sc_clock clk("clk", CLK_TIME, sc_core::SC_SEC);
  sc_core::sc_signal<sc_uint<8> > s_tx_red;
  sc_core::sc_signal<sc_uint<8> > s_tx_green;
  sc_core::sc_signal<sc_uint<8> > s_tx_blue;
  // -- Outputs
  sc_core::sc_signal<bool> s_hsync;
  sc_core::sc_signal<bool> s_vsync;
  sc_core::sc_signal<unsigned int> s_h_count;
  sc_core::sc_signal<unsigned int> s_v_count;
  sc_core::sc_signal<sc_uint<8> > s_rx_red;
  sc_core::sc_signal<sc_uint<8> > s_rx_green;
  sc_core::sc_signal<sc_uint<8> > s_rx_blue;

  // VGA module instanciation and connections
  vga<
    BITS,
    H_ACTIVE, H_FP, H_SYNC_PULSE, H_BP,
    V_ACTIVE, V_FP, V_SYNC_PULSE, V_BP
  > ips_vga("ips_vga");
  ips_vga.clk(clk);
  ips_vga.red(s_tx_red);
  ips_vga.green(s_tx_green);
  ips_vga.blue(s_tx_blue);
  ips_vga.o_hsync(s_hsync);
  ips_vga.o_vsync(s_vsync);
  ips_vga.o_h_count(s_h_count);
  ips_vga.o_v_count(s_v_count);
  ips_vga.o_red(s_rx_red);
  ips_vga.o_green(s_rx_green);
  ips_vga.o_blue(s_rx_blue);

  // Signals to dump
#ifdef IPS_DUMP_EN
  sc_trace_file* wf = sc_create_vcd_trace_file("ips_vga");

  sc_trace(wf, clk, "clk");
  sc_trace(wf, s_tx_red, "tx_red");
  sc_trace(wf, s_tx_green, "tx_green");
  sc_trace(wf, s_tx_blue, "tx_blue");
  sc_trace(wf, s_hsync, "hsync");
  sc_trace(wf, s_vsync, "vsync");
  sc_trace(wf, s_h_count, "h_count");
  sc_trace(wf, s_v_count, "v_count");
  sc_trace(wf, s_rx_red, "rx_red");
  sc_trace(wf, s_rx_green, "rx_green");
  sc_trace(wf, s_rx_blue, "rx_blue");
#endif // IPS_DUMP_EN

  // Start time
  std::cout << "@" << sc_time_stamp() << std::endl;

  double total_sim_time = 0.0;

  while (SIM_TIME > total_sim_time)
  {
    const int IMG_ROW = s_v_count.read() - (V_SYNC_PULSE + V_BP);
    const int IMG_COL = s_h_count.read() - (H_SYNC_PULSE + H_BP);

#ifdef IPS_DEBUG_EN
    std::cout << "TX image: ";
    std::cout << "row = " << IMG_ROW;
    std::cout << " col = " << IMG_COL;
#endif // IPS_DEBUG_EN

    if ((IMG_ROW < 0) || (IMG_COL < 0) || (IMG_ROW >= V_ACTIVE) || (IMG_COL >= H_ACTIVE))
    {
      s_tx_red.write(0);
      s_tx_green.write(0);
      s_tx_blue.write(0);
      
#ifdef IPS_DEBUG_EN
      std::cout << " dpixel = (0,0,0) " << std::endl;
#endif // IPS_DEBUG_EN
    }
    else
    {
      cv::Vec3b pixel = tx_img.at<cv::Vec3b>(IMG_ROW, IMG_COL, 0);

      s_tx_red.write(static_cast<sc_uint<8> >(pixel[0]));
      s_tx_green.write(static_cast<sc_uint<8> >(pixel[1]));
      s_tx_blue.write(static_cast<sc_uint<8> >(pixel[2]));
      
#ifdef IPS_DEBUG_EN
      std::cout << " ipixel = (" << static_cast<int>(pixel[0]) << ","
        << static_cast<int>(pixel[1]) << "," << static_cast<int>(pixel[2])
        << ")" << std::endl;
#endif // IPS_DEBUG_EN
    }

    total_sim_time += CLK_TIME;
    sc_start(CLK_TIME, sc_core::SC_SEC);

    if ((IMG_ROW < 0) || (IMG_COL < 0) || (IMG_ROW >= V_ACTIVE) || (IMG_COL >= H_ACTIVE))
    {
      cv::Vec3b pixel(0, 0, 0);
      rx_data.at<cv::Vec3b>(s_v_count.read(), s_h_count.read()) = pixel;
    }
    else
    {
      cv::Vec3b pixel = cv::Vec3b(s_rx_red.read(), s_rx_green.read(), s_rx_blue.read());
      rx_data.at<cv::Vec3b>(s_v_count.read(), s_h_count.read()) = pixel;
      rx_img.at<cv::Vec3b>(IMG_ROW, IMG_COL) = pixel;
    }
  }

  // End time
  std::cout << "@" << sc_time_stamp() << std::endl;

#ifdef IPS_DUMP_EN
  sc_close_vcd_trace_file(wf);
#endif // IPS_DUMP_EN

#ifdef IPS_IMSHOW
  // Show the images in their respective windows
  cv::imshow("TX image", tx_img);
  cv::imshow("RX image", rx_img);

  // Wait for a key press indefinitely to keep the windows open
  cv::waitKey(0);
#endif // IPS_IMSHOW

  return 0;
}
