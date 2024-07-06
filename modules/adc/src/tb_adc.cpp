#include <systemc-ams.h>
#include "adc.hpp"
#include "test_adc.hpp"

#define N 8


int sc_main(int, char*[])
{
  sca_tdf::sca_signal<double> s_ana;
  sca_tdf::sca_signal<sc_dt::sc_uint<N> > s_dig_out;

  const int MAX_ITER = (1 << N) - 1;

  dac<N> ips_adc("ips_adc");
  ips_adc.in(s_ana);
  ips_adc.out(s_dig_out);

  test_adc<N> ips_test_adc("ips_test_adc");
  ips_test_adc.o_ana(s_ana);

  sca_util::sca_trace_file* tf = sca_util::sca_create_vcd_trace_file("ips_adc");
  sca_util::sca_trace(tf, s_ana, "in");
  sca_util::sca_trace(tf, s_dig_out, "out");

  sc_start(MAX_ITER * 0.1, SC_US);

  sca_util::sca_close_vcd_trace_file(tf);

  return 0;
};
