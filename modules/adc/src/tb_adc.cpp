#ifndef USING_TLM_TB_EN
#include <systemc-ams.h>
#include "adc.hpp"
#include "seq_item_adc.hpp"

#define N 8
#define VOLTAGE_MIN 0
#define VOLTAGE_MAX 3300


int sc_main(int, char*[])
{
  // Max number of sequence items to test
  const int MAX_SEQ_ITEMS = (1 << N) - 1;

  // Signals to connect
  sca_tdf::sca_signal<double> s_ana;
  sca_tdf::sca_signal<sc_dt::sc_uint<N> > s_dig_out;

  // DUT
  adc<N, VOLTAGE_MIN, VOLTAGE_MAX, VUnit::mv> ips_adc("ips_adc");
  ips_adc.in(s_ana);
  ips_adc.out(s_dig_out);

  // Sequence item generator for ADC
  seq_item_adc<N, VOLTAGE_MIN, VOLTAGE_MAX, VUnit::mv> ips_seq_item_adc("ips_seq_item_adc");
  ips_seq_item_adc.o_ana(s_ana);

  // Dump waveform
  sca_util::sca_trace_file* tf = sca_util::sca_create_vcd_trace_file("ips_adc");
  sca_util::sca_trace(tf, s_ana, "in");
  sca_util::sca_trace(tf, s_dig_out, "out");

  // Start time
  std::cout << "@" << sc_time_stamp() << std::endl;

  // Run test
  sc_start(MAX_SEQ_ITEMS * 13, SC_NS);

  // End time
  std::cout << "@" << sc_time_stamp() << std::endl;

  sca_util::sca_close_vcd_trace_file(tf);

  return 0;
};
#endif // USING_TLM_TB_EN
