#include <systemc.h>
#include <systemc-ams.h>

// #ifndef IPS_AMS
// #define IPS_AMS
// #endif // IPS_AMS

#include "adc.hpp"
#include "dac.hpp"
#include "memory.hpp"
#include "seq_item_ams.hpp"
#include "vga.hpp"

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
#define VOLTAGE_MIN 0
#define VOLTAGE_MAX 3300
// Memory parameters
#define IMG_INPUT_ADDR 0x00000034u
#define MEM_SIZE 0x002A3034u

int sc_main(int, char *[])
{
  // Compute the clock time in seconds
  const double CLK_TIME = 1.0 / static_cast<double>(CLK_FREQ);
  // Compute the total simulation based on the total amount of pixels in the
  // screen
  const double SIM_TIME = CLK_TIME * static_cast<double>(TOTAL_PIXELES);

  // Signals to use
  // -- Inputs of VGA
  sc_core::sc_clock clk("clk", CLK_TIME, sc_core::SC_SEC);
  sc_core::sc_signal<sc_uint<BITS>> s_tx_red;
  sc_core::sc_signal<sc_uint<BITS>> s_tx_green;
  sc_core::sc_signal<sc_uint<BITS>> s_tx_blue;
  // -- Outputs of VGA
  sc_core::sc_signal<bool> s_hsync;
  sc_core::sc_signal<bool> s_vsync;
  sc_core::sc_signal<unsigned int> s_h_count;
  sc_core::sc_signal<unsigned int> s_v_count;
  sc_core::sc_signal<sc_uint<BITS>> s_rx_red;
  sc_core::sc_signal<sc_uint<BITS>> s_rx_green;
  sc_core::sc_signal<sc_uint<BITS>> s_rx_blue;
  // -- Outputs of DAC
  sca_tdf::sca_signal<double> s_ana_red;
  sca_tdf::sca_signal<double> s_ana_green;
  sca_tdf::sca_signal<double> s_ana_blue;
  // -- Outputs of ADC
  sc_core::sc_signal<sc_uint<BITS>> s_dig_out_red;
  sc_core::sc_signal<sc_uint<BITS>> s_dig_out_green;
  sc_core::sc_signal<sc_uint<BITS>> s_dig_out_blue;
  // -- Memory
  sc_core::sc_signal<bool> s_we;
  sc_core::sc_signal<unsigned long long int> s_address;
  sc_core::sc_signal<sc_uint<24>> s_wdata;
  sc_core::sc_signal<sc_uint<24>> s_rdata;

  // Data generation for the VGA pixels
  seq_item_ams<
      BITS,
      H_ACTIVE, H_FP, H_SYNC_PULSE, H_BP,
      V_ACTIVE, V_FP, V_SYNC_PULSE, V_BP>
      ips_seq_item_ams("ips_seq_item_ams");
  ips_seq_item_ams.clk(clk);
  ips_seq_item_ams.hcount(s_h_count);
  ips_seq_item_ams.vcount(s_v_count);
  ips_seq_item_ams.o_red(s_tx_red);
  ips_seq_item_ams.o_green(s_tx_green);
  ips_seq_item_ams.o_blue(s_tx_blue);

  // VGA module instanciation and connections
  vga<
      BITS,
      H_ACTIVE, H_FP, H_SYNC_PULSE, H_BP,
      V_ACTIVE, V_FP, V_SYNC_PULSE, V_BP>
      ips_vga("ips_vga");
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

  // DAC module instanciations and connections
  dac<BITS, VOLTAGE_MIN, VOLTAGE_MAX, VUnit::mv> ips_dac_red("ips_dac_red");
  ips_dac_red.in(s_rx_red);
  ips_dac_red.out(s_ana_red);

  dac<BITS, VOLTAGE_MIN, VOLTAGE_MAX, VUnit::mv> ips_dac_green("ips_dac_green");
  ips_dac_green.in(s_rx_green);
  ips_dac_green.out(s_ana_green);

  dac<BITS, VOLTAGE_MIN, VOLTAGE_MAX, VUnit::mv> ips_dac_blue("ips_dac_blue");
  ips_dac_blue.in(s_rx_blue);
  ips_dac_blue.out(s_ana_blue);

  // ADC module instanciations and connections
  adc<BITS, VOLTAGE_MIN, VOLTAGE_MAX, VUnit::mv> ips_adc_red("ips_adc_red");
  ips_adc_red.in(s_ana_red);
  ips_adc_red.out(s_dig_out_red);

  adc<BITS, VOLTAGE_MIN, VOLTAGE_MAX, VUnit::mv> ips_adc_green("ips_adc_green");
  ips_adc_green.in(s_ana_green);
  ips_adc_green.out(s_dig_out_green);

  adc<BITS, VOLTAGE_MIN, VOLTAGE_MAX, VUnit::mv> ips_adc_blue("ips_adc_blue");
  ips_adc_blue.in(s_ana_blue);
  ips_adc_blue.out(s_dig_out_blue);

  memory<MEM_SIZE> ips_memory("ips_memory");
  ips_memory.clk(clk);
  ips_memory.address(s_address);
  ips_memory.we(s_we);
  ips_memory.wdata(s_wdata);
  ips_memory.rdata(s_rdata);

  // Signals to dump
#ifdef IPS_DUMP_EN
  sca_util::sca_trace_file *wf = sca_util::sca_create_vcd_trace_file("ips_ams");

  sca_trace(wf, clk, "clk");
  sca_trace(wf, s_hsync, "hsync");
  sca_trace(wf, s_vsync, "vsync");
  sca_trace(wf, s_h_count, "h_count");
  sca_trace(wf, s_v_count, "v_count");
  sca_trace(wf, s_tx_red, "tx_red");
  sca_trace(wf, s_tx_green, "tx_green");
  sca_trace(wf, s_tx_blue, "tx_blue");
  sca_trace(wf, s_rx_red, "rx_red");
  sca_trace(wf, s_rx_green, "rx_green");
  sca_trace(wf, s_rx_blue, "rx_blue");
  sca_trace(wf, s_ana_red, "ana_red");
  sca_trace(wf, s_ana_green, "ana_green");
  sca_trace(wf, s_ana_blue, "ana_blue");
  sca_trace(wf, s_dig_out_red, "to_mem_red");
  sca_trace(wf, s_dig_out_green, "to_mem_green");
  sca_trace(wf, s_dig_out_blue, "to_mem_blue");
  sca_trace(wf, s_we, "we");
  sca_trace(wf, s_address, "address");
  sca_trace(wf, s_rdata, "rdata");
#endif // IPS_DUMP_EN

  // Start time
  std::cout << "@" << sc_time_stamp() << std::endl;

  double total_sim_time = 0.0;

  while (SIM_TIME > total_sim_time)
  {
    const int IMG_ROW = s_v_count.read() - (V_SYNC_PULSE + V_BP);
    const int IMG_COL = s_h_count.read() - (H_SYNC_PULSE + H_BP);

    if ((IMG_ROW < 0) || (IMG_COL < 0) || (IMG_ROW >= V_ACTIVE) || (IMG_COL >= H_ACTIVE))
    {
      s_we.write(false);
    }
    else
    {
      const unsigned long long ADDR = static_cast<unsigned long long>(IMG_INPUT_ADDR + IMG_ROW * V_ACTIVE + IMG_COL);
      s_address.write(ADDR);
      s_wdata.write((s_dig_out_red.read() << 16) + (s_dig_out_green.read() << 8) + s_dig_out_blue.read());
      s_we.write(true);

#ifdef IPS_DEBUG_EN
      std::cout << " MEM[" << ADDR << "] = " << s_dig_out_blue.read() << std::endl
                << " MEM[" << (ADDR + 1) << "] = " << s_dig_out_green.read() << std::endl
                << " MEM[" << (ADDR + 2) << "] = " << s_dig_out_red.read() << std::endl;
#endif // IPS_DEBUG_EN
    }

    total_sim_time += CLK_TIME;
    sc_start(CLK_TIME, sc_core::SC_SEC);
  }

  // End time
  std::cout << "@" << sc_time_stamp() << std::endl;

#ifdef IPS_DUMP_EN
  sca_util::sca_close_vcd_trace_file(wf);
#endif // IPS_DUMP_EN

  return 0;
}
