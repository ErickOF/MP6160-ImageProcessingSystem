#ifndef IPS_VGA_MODEL_HPP
#define IPS_VGA_MODEL_HPP

#include <systemc.h>
#ifdef IPS_AMS
#include <systemc-ams.h>
#endif // IPS_AMS

#define IPS_VGA_ACTIVE true
#define IPS_VGA_INACTIVE false

// Main clock frequency in Hz - 25.175 MHz
#define CLK_FREQ 25175000

/**
 * @brief VGA representation class
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
  unsigned int V_BP = 33
>
SC_MODULE(vga)
{
protected:
  // Horizontal count
  int h_count;
  // Vertical count
  int v_count;
public:
#ifndef USING_TLM_TB_EN
  // Input clock
  sc_core::sc_in<bool> clk;
#else
  // Compute the clock time in seconds
  const double CLK_TIME = 1.0 / static_cast<double>(CLK_FREQ);
  // Internal clock
  sc_core::sc_clock clk;
#endif // USING_TLM_TB_EN
  // Input pixel
  sc_core::sc_in<sc_uint<BITS> > red;
  sc_core::sc_in<sc_uint<BITS> > green;
  sc_core::sc_in<sc_uint<BITS> > blue;
  // Counter outputs
  sc_core::sc_out<unsigned int> o_h_count;
  sc_core::sc_out<unsigned int> o_v_count;
  // Output horizontal sync
  sc_core::sc_out<bool> o_hsync;
  // Output vertical sync
  sc_core::sc_out<bool> o_vsync;
#ifndef USING_TLM_TB_EN
  // Output pixel
  sc_core::sc_out<sc_uint<BITS> > o_red;
  sc_core::sc_out<sc_uint<BITS> > o_green;
  sc_core::sc_out<sc_uint<BITS> > o_blue;
#else
  unsigned char *tmp_img;
  bool start;
  bool done;
#endif // USING_TLM_TB_EN

  SC_CTOR(vga)
    : o_hsync("o_hsync"), o_vsync("o_vsync")
#ifdef USING_TLM_TB_EN
    , clk("clk", CLK_TIME, sc_core::SC_SEC)
#endif
  {
    this->h_count = 0;
    this->v_count = 0;

    SC_METHOD(run);
#ifndef USING_TLM_TB_EN
    sensitive << clk.pos();
#else
    sensitive << clk;
    this->tmp_img = new unsigned char[H_ACTIVE * V_ACTIVE * 3];
    this->start = false;
    this->done = false;
#endif // USING_TLM_TB_EN
  }

  /**
   * @brief Override method
   * Compute the values output of the VGA
   * 
   */
  void run()
  {
    if (this->clk.read())
    {
#ifdef USING_TLM_TB_EN
    if (this->start)
    {
#endif // USING_TLM_TB_EN
#ifdef IPS_DEBUG_EN
      std::cout << "@" << sc_core::sc_time_stamp().to_seconds() * 1e6 << "us" << std::endl;
#endif // IPS_DEBUG_EN

      // Increment H counter
      this->h_count++;

      // HSYNC pulse
      if (this->h_count == H_SYNC_PULSE)
      {
        this->o_hsync.write(IPS_VGA_ACTIVE);
      }
      else if (this->h_count == (H_SYNC_PULSE + H_BP))
      {
        this->o_hsync.write(IPS_VGA_ACTIVE);
      }
      else if (this->h_count == (H_SYNC_PULSE + H_BP + H_ACTIVE))
      {
        this->o_hsync.write(IPS_VGA_ACTIVE);
      }
      // End of HSYNC
      else if (this->h_count == (H_SYNC_PULSE + H_BP + H_ACTIVE + H_FP))
      {
        // Restart H counter
        this->o_hsync.write(IPS_VGA_INACTIVE);
        this->h_count = 0;

        // Increment H counter
        this->v_count++;

        // VSYNC pulse
        if (this->v_count == V_SYNC_PULSE)
        {
          this->o_vsync.write(IPS_VGA_ACTIVE);
        }
        // End of V-sync pulse
        else if (this->v_count == (V_SYNC_PULSE + V_BP))
        {
          this->o_vsync.write(IPS_VGA_ACTIVE);
        }
        // V front porch
        else if (this->v_count == (V_SYNC_PULSE + V_BP + V_ACTIVE))
        {
          this->o_vsync.write(IPS_VGA_ACTIVE);
        }
        // End of VSYNC
        else if (this->v_count == (V_SYNC_PULSE + V_BP + V_ACTIVE + V_FP))
        {
          this->o_vsync.write(IPS_VGA_INACTIVE);
          this->v_count = 0;
          this->done = true;
          this->start = false;
        }
      }

      this->o_v_count.write(this->v_count);
      this->o_h_count.write(this->h_count);
#ifndef USING_TLM_TB_EN
      this->o_red.write(this->red.read());
      this->o_green.write(this->green.read());
      this->o_blue.write(this->blue.read());
#else
      const int IMG_ROW = this->v_count - (V_SYNC_PULSE + V_BP);
      const int IMG_COL = this->h_count - (H_SYNC_PULSE + H_BP);

      if (!((IMG_ROW < 0) || (IMG_COL < 0) || (IMG_ROW >= V_ACTIVE) || (IMG_COL >= H_ACTIVE)))
      {
        const unsigned int INDEX = IMG_ROW * (3 * H_ACTIVE) + 3 * IMG_COL;

        this->tmp_img[INDEX] = this->red.read();
        this->tmp_img[INDEX + 1] = this->green.read();
        this->tmp_img[INDEX + 2] = this->blue.read();
      }
    }
#endif // USING_TLM_TB_EN
    }
  }
};
#endif // IPS_VGA_MODEL_HPP
