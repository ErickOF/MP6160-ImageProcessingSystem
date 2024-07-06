#include <systemc-ams.h>
#include "dac.hpp"
#include "seq_item_dac.hpp"

#define N 8


int sc_main(int, char*[])
{
  // Max number of sequence items to test
  const int MAX_SEQ_ITEMS = (1 << N) - 1;

  // Signals to connect
  sca_tdf::sca_signal<sc_dt::sc_uint<N> > s_dig;
  sca_tdf::sca_signal<double> s_ana_out;

  // DUT
  dac<N> ips_dac("ips_dac");
  ips_dac.in(s_dig);
  ips_dac.out(s_ana_out);

  // Sequence item generator for DAC
  seq_item_dac<N> ips_seq_item_dac("ips_seq_item_dac");
  ips_seq_item_dac.o_dig(s_dig);

  // Dump waveform
  sca_util::sca_trace_file* tf = sca_util::sca_create_vcd_trace_file("ips_dac");
  sca_util::sca_trace(tf, s_dig, "in");
  sca_util::sca_trace(tf, s_ana_out, "out");

  // Start time
  std::cout << "@" << sc_time_stamp() << std::endl;

  // Run test
  sc_start(MAX_SEQ_ITEMS * 0.1, SC_US);

  // End time
  std::cout << "@" << sc_time_stamp() << std::endl;

  sca_util::sca_close_vcd_trace_file(tf);

  return 0;
};
