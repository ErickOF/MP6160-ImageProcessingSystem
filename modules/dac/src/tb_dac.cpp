#include <systemc-ams.h>
#include "dac.hpp"
#include "test_dac.hpp"

#define N 8


int sc_main(int, char*[])
{
  sca_tdf::sca_signal<sc_dt::sc_uint<N> > s_dig;
  sca_tdf::sca_signal<double> s_ana_out;

  const int MAX_ITER = (1 << N) - 1;

  dac<N> ips_dac("ips_dac");
  ips_dac.in(s_dig);
  ips_dac.out(s_ana_out);

  test_dac<N> ips_test_dac("ips_test_dac");
  ips_test_dac.o_dig(s_dig);

  sca_util::sca_trace_file* tf = sca_util::sca_create_vcd_trace_file("ips_dac");
  sca_util::sca_trace(tf, s_dig, "in");
  sca_util::sca_trace(tf, s_ana_out, "out");

  sc_start(MAX_ITER * 0.1, SC_US);

  sca_util::sca_close_vcd_trace_file(tf);

  return 0;
};
