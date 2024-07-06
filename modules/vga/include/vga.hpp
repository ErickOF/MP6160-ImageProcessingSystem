#ifndef IPS_VGA_MODEL_HPP
#define IPS_VGA_MODEL_HPP

#include <systemc.h>

#define IPS_VGA_ACTIVE true
#define IPS_VGA_INACTIVE false

/**
 * @brief VGA representation class
 * 
 * @tparam CLK_FREQ - clock frequency in HHz of the VGA
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
  // Input clock
  sc_core::sc_in<bool> clk;
  // Output horizontal synch
  sc_core::sc_out<bool> o_hsync;
  // Output vertical synch
  sc_core::sc_out<bool> o_vsync;
#ifdef IPS_DEBUG_EN
  // For debug
  sc_core::sc_out<int> o_h_count;
  sc_core::sc_out<int> o_v_count;
#endif // IPS_DEBUG_EN

  SC_CTOR(vga) : o_hsync("o_hsync"), o_vsync("o_vsync")
  {
    this->h_count = 0;
    this->v_count = 0;

    SC_METHOD(run);
    sensitive << clk.pos();
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
      // End of H-sync pulse
      else if (this->h_count == (H_SYNC_PULSE + H_BP))
      {
        this->o_hsync.write(IPS_VGA_ACTIVE);
      }
      // H front porch
      else if (this->h_count == (H_SYNC_PULSE + H_BP + H_ACTIVE))
      {
        this->o_hsync.write(IPS_VGA_INACTIVE);
      }
      // End of HSYNC
      else if (this->h_count == (H_SYNC_PULSE + H_BP + H_ACTIVE + H_FP))
      {
        // Restart H counter
        this->o_hsync.write(IPS_VGA_ACTIVE);
        this->h_count = 0;

        // Increment H counter
        this->v_count++;

        // VSYNC pulse
        if (this->v_count == V_SYNC_PULSE)
        {
          this->o_vsync.write(IPS_VGA_INACTIVE);
        }
        // End of V-sync pulse
        else if (this->v_count == (V_SYNC_PULSE + V_BP))
        {
          this->o_vsync.write(IPS_VGA_ACTIVE);
        }
        // V front porch
        else if (this->v_count == (V_SYNC_PULSE + V_BP + V_ACTIVE))
        {
          this->o_vsync.write(IPS_VGA_INACTIVE);
        }
        // End of VSYNC
        else if (this->v_count == (V_SYNC_PULSE + V_BP + V_ACTIVE + V_FP))
        {
          this->o_vsync.write(IPS_VGA_ACTIVE);
          this->v_count = 0;
        }
      }

#ifdef IPS_DEBUG_EN
      this->o_v_count.write(this->v_count);
      this->o_h_count.write(this->h_count);
#endif // IPS_DEBUG_EN
    }
  }
};
#endif // IPS_VGA_MODEL_HPP
