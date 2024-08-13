#ifndef IPS_SEQ_ITEM_AMS_HPP
#define IPS_SEQ_ITEM_AMS_HPP

#define int64 systemc_int64
#define uint64 systemc_uint64
#include <systemc.h>
#include <systemc-ams.h>
#undef int64
#undef uint64
#define int64 opencv_int64
#define uint64 opencv_uint64
#include <opencv2/opencv.hpp>
#undef int64
#undef uint64

// Image path
#define IPS_IMG_PATH_TB "../../tools/datagen/src/imgs/car_rgb_noisy_image.jpg"

/**
 * @brief This class is used to generate the data for the AMS test
 *
 * @tparam N - the number of output bits of the digital pixel
 * @tparam H_ACTIVE - output horizontal active video pixels
 * @tparam H_FP - wait after the display period before the sync
 *  horizontal pulse
 * @tparam H_SYNC_PULSE - assert HSYNC
 * @tparam H_BP - wait after the sync horizontal pulse before starting
 *  the next display period
 * @tparam V_ACTIVE - output vertical active video pixels
 * @tparam V_FP - wait after the display period before the sync
 *  vertical pulse
 * @tparam V_SYNC_PULSE - assert VSYNC
 * @tparam V_BP - wait after the sync vertical pulse before starting
 *  the next display period
 */
template <
  unsigned int N = 8,
  unsigned int H_ACTIVE = 640,
  unsigned int H_FP = 16,
  unsigned int H_SYNC_PULSE = 96,
  unsigned int H_BP = 48,
  unsigned int V_ACTIVE = 480,
  unsigned int V_FP = 10,
  unsigned int V_SYNC_PULSE = 2,
  unsigned int V_BP = 33
>
SC_MODULE(seq_item_ams)
{
protected:
  cv::Mat tx_img;

public:
  // Input clock
  sc_core::sc_in<bool> clk;
  // Counters
  sc_core::sc_in<unsigned int> hcount;
  sc_core::sc_in<unsigned int> vcount;
  // Output pixel
  sc_core::sc_out<sc_uint<N> > o_red;
  sc_core::sc_out<sc_uint<N> > o_green;
  sc_core::sc_out<sc_uint<N> > o_blue;

  SC_CTOR(seq_item_ams)
  {
    // Read image
    const std::string img_path = IPS_IMG_PATH_TB;

    cv::Mat read_img = cv::imread(img_path, cv::IMREAD_COLOR);

    // CV_8UC3 Type: 8-bit unsigned, 3 channels (e.g., for a color image)
    read_img.convertTo(this->tx_img, CV_8UC3);

#ifdef IPS_DEBUG_EN
    std::cout << "Loading image: " << img_path << std::endl;
#endif // IPS_DEBUG_EN

    // Check if the image is loaded successfully
    if (this->tx_img.empty())
    {
      std::cerr << "Error: Could not open or find the image!" << std::endl;
      exit(EXIT_FAILURE);
    }

#ifdef IPS_DEBUG_EN
    std::cout << "TX image info: ";
    std::cout << "rows = " << this->tx_img.rows;
    std::cout << " cols = " << this->tx_img.cols;
    std::cout << " channels = " << this->tx_img.channels() << std::endl;
#endif // IPS_DEBUG_EN

    SC_METHOD(run);
    sensitive << clk.pos();
  }

  void run()
  {
    if (this->clk.read())
    {
      const int IMG_ROW = static_cast<int>(this->vcount.read()) - (V_SYNC_PULSE + V_BP);
      const int IMG_COL = static_cast<int>(this->hcount.read()) - (H_SYNC_PULSE + H_BP);

#ifdef IPS_DEBUG_EN
      std::cout << "TX image: ";
      std::cout << "row = " << IMG_ROW;
      std::cout << " col = " << IMG_COL;
#endif // IPS_DEBUG_EN

      if ((IMG_ROW < 0) || (IMG_COL < 0) || (IMG_ROW >= static_cast<int>(V_ACTIVE)) || (IMG_COL >= static_cast<int>(H_ACTIVE)))
      {
        this->o_red.write(0);
        this->o_green.write(0);
        this->o_blue.write(0);

#ifdef IPS_DEBUG_EN
        std::cout << " dpixel = (0,0,0) " << std::endl;
#endif // IPS_DEBUG_EN
      }
      else
      {
        if ((IMG_ROW >= this->tx_img.rows) || (IMG_COL >= this->tx_img.cols))
        {
          this->o_red.write(0);
          this->o_green.write(0);
          this->o_blue.write(0);
        }
        else
        {
          cv::Vec3b pixel = tx_img.at<cv::Vec3b>(IMG_ROW, IMG_COL, 0);

          this->o_red.write(static_cast<sc_uint<8>>(pixel[0]));
          this->o_green.write(static_cast<sc_uint<8>>(pixel[1]));
          this->o_blue.write(static_cast<sc_uint<8>>(pixel[2]));
        }

#ifdef IPS_DEBUG_EN
        std::cout << " ipixel = (" << static_cast<int>(pixel[0]) << ","
                  << static_cast<int>(pixel[1]) << "," << static_cast<int>(pixel[2])
                  << ")" << std::endl;
#endif // IPS_DEBUG_EN
      }
    }
  }
};

#endif // IPS_SEQ_ITEM_AMS_HPP
