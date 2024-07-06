#ifndef IPS_TEST_DAC_HPP
#define IPS_TEST_DAC_HPP

#include <cstdlib>


template <unsigned int N>
SCA_TDF_MODULE(test_dac) {
public:
  sca_tdf::sca_out<sc_dt::sc_uint<N> > o_dig;

  SCA_CTOR(test_dac) {
    set_timestep(sca_core::sca_time(0.1, sc_core::SC_US));
  }

  void processing()
  {
    this->o_dig.write(static_cast<sc_dt::sc_uint<N> >(rand() % (1 << N)));
  }
};

#endif // IPS_TEST_DAC_HPP
