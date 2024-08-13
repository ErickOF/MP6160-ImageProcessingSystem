#ifndef IPS_SEQ_ITEM_BGA_HPP
#define IPS_SEQ_ITEM_BGA_HPP

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

#include "vunit.hpp"

// Image path
#define IPS_IMG_PATH_TB "../../tools/datagen/src/imgs/car_rgb_noisy_image.jpg"

/**
 * @brief This class is used to generate the data for the AMS test
 *
 * @tparam BITS - the number of output bits of the digital pixel
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
    unsigned int BITS = 8,
    unsigned int H_ACTIVE = 640,
    unsigned int H_FP = 16,
    unsigned int H_SYNC_PULSE = 96,
    unsigned int H_BP = 48,
    unsigned int V_ACTIVE = 480,
    unsigned int V_FP = 10,
    unsigned int V_SYNC_PULSE = 2,
    unsigned int V_BP = 33,
    int VMIN = 0,
    int VMAX = 5,
    VUnit VU = VUnit::v>
SCA_TDF_MODULE(seq_item_vga)
{
protected:
  cv::Mat tx_img;
  // Min voltage value based on the voltage units
  const double V_MIN = static_cast<double>(VMIN) / static_cast<double>(VU);
  // Max voltage value based on the voltage units
  const double V_MAX = static_cast<double>(VMAX) / static_cast<double>(VU);
  // Max digital output code
  const double MAX_DIG = static_cast<double>((1 << BITS) - 1);

public:
  // Counters
  sca_tdf::sca_de::sca_in<unsigned int> hcount;
  sca_tdf::sca_de::sca_in<unsigned int> vcount;
  // Output pixel
  sca_tdf::sca_out<double> o_red;
  sca_tdf::sca_out<double> o_green;
  sca_tdf::sca_out<double> o_blue;

  SC_CTOR(seq_item_vga)
  {
    // Read image
    const std::string img_path = IPS_IMG_PATH_TB;

    cv::Mat read_img = cv::imread(img_path, cv::IMREAD_COLOR);

    // CV_8UC3 Type: 8-bit unsigned, 3 channels (e.g., for a color image)
    read_img.convertTo(this->tx_img, CV_8UC3);

    // Check if the image is loaded successfully
    if (this->tx_img.empty())
    {
      std::cerr << "Error: Could not open or find the image!" << std::endl;
      exit(EXIT_FAILURE);
    }
  }

  void set_attributes()
  {
    // Propagation time from input to output
    set_timestep(sca_core::sca_time(1, sc_core::SC_NS));
    this->o_red.set_delay(17);
    this->o_green.set_delay(17);
    this->o_blue.set_delay(17);
  }

  void processing()
  {
    const int IMG_ROW = static_cast<int>(this->vcount.read()) - (V_SYNC_PULSE + V_BP);
    const int IMG_COL = static_cast<int>(this->hcount.read()) - (H_SYNC_PULSE + H_BP);

    if ((IMG_ROW < 0) || (IMG_COL < 0) || (IMG_ROW >= static_cast<int>(V_ACTIVE)) || (IMG_COL >= static_cast<int>(H_ACTIVE)))
    {
      this->o_red.write(0.0);
      this->o_green.write(0.0);
      this->o_blue.write(0.0);
    }
    else
    {
      if ((IMG_ROW >= this->tx_img.rows) || (IMG_COL >= this->tx_img.cols))
      {
        this->o_red.write(0.0);
        this->o_green.write(0.0);
        this->o_blue.write(0.0);
      }
      else
      {
        cv::Vec3b pixel = tx_img.at<cv::Vec3b>(IMG_ROW, IMG_COL, 0);

        this->o_red.write(sc_uint2double(static_cast<sc_uint<BITS>>(pixel[0])));
        this->o_green.write(sc_uint2double(static_cast<sc_uint<BITS>>(pixel[1])));
        this->o_blue.write(sc_uint2double(static_cast<sc_uint<BITS>>(pixel[2])));
      }
    }
  }

  /**
   * @brief Convert the digital signal into analog signal
   * The N-bit digital code is converted into an analog signal in a voltage
   * range from Vmin to Vmax
   */
  double sc_uint2double(sc_uint<BITS> in)
  {
    double dig_in = static_cast<double>(in);
    return V_MIN + (dig_in / MAX_DIG) * (V_MAX - V_MIN);
  }
};

#endif // IPS_SEQ_ITEM_BGA_HPP
