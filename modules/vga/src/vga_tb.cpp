#include <systemc.h>

#include "vga.hpp"

// 120 MHz
#define CLK_FREQ 120000000
#define H_ACTIVE 640
#define H_FP 16
#define H_SYNC_PULSE 96
#define H_BP 48
#define V_ACTIVE 480
#define V_FP 10
#define V_SYNC_PULSE 2
#define V_BP 33
#define TOTAL_PIXELES ((H_ACTIVE + H_FP + H_SYNC_PULSE + H_BP) *\
                       (V_ACTIVE + V_FP + V_SYNC_PULSE + V_BP))


int sc_main(int, char*[])
{
  const double CLK_TIME = 1.0 / static_cast<double>(CLK_FREQ);
  const double SIM_TIME = CLK_TIME * static_cast<double>(TOTAL_PIXELES);

  sc_time total_sim_time = sc_time(SIM_TIME, SC_SEC);

  sc_core::sc_clock s_clk("clk", CLK_TIME, sc_core::SC_SEC);
  sc_core::sc_signal<bool> s_hsync;
  sc_core::sc_signal<bool> s_vsync;

#ifdef IPS_DEBUG_EN
  sc_core::sc_signal<int> s_h_count;
  sc_core::sc_signal<int> s_v_count;
#endif // IPS_DEBUG_EN

  vga<
    H_ACTIVE, H_FP, H_SYNC_PULSE, H_BP,
    V_ACTIVE, V_FP, V_SYNC_PULSE, V_BP
  > ips_vga("ips_vga");
  ips_vga.clk(s_clk);
  ips_vga.o_hsync(s_hsync);
  ips_vga.o_vsync(s_vsync);
#ifdef IPS_DEBUG_EN
  ips_vga.o_h_count(s_h_count);
  ips_vga.o_v_count(s_v_count);
#endif // IPS_DEBUG_EN

#ifdef IPS_DUMP_EN
  sca_util::sca_trace_file* tf = sca_util::sca_create_vcd_trace_file("ips_vga");
  sca_trace(tf, s_clk, "clk");
  sca_trace(tf, s_hsync, "hsync");
  sca_trace(tf, s_vsync, "vsync");
#ifdef IPS_DEBUG_EN
  sca_trace(tf, s_h_count, "h_count");
  sca_trace(tf, s_v_count, "v_count");
#endif // IPS_DEBUG_EN
#endif // IPS_DUMP_EN

  std::cout << "@" << sc_time_stamp() << std::endl;
  std::cout << "Expected total sim time: " << total_sim_time << std::endl;

  sc_start(SIM_TIME, sc_core::SC_SEC);

  std::cout << "@" << sc_time_stamp() << std::endl;

#ifdef IPS_DUMP_EN
  sca_util::sca_close_vcd_trace_file(tf);
#endif // IPS_DUMP_EN

  return 0;
}
