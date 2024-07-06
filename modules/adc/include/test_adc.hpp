#ifndef IPS_TEST_ADC_HPP
#define IPS_TEST_ADC_HPP

#include <cstdlib>


template <unsigned int N>
SCA_TDF_MODULE(test_adc) {
public:
  sca_tdf::sca_out<double> o_ana;
  const int MAX_CODE = (1 << N);

  SCA_CTOR(test_adc) {
    set_timestep(sca_core::sca_time(0.1, sc_core::SC_US));
  }

  void processing()
  {
    this->o_ana.write(static_cast<double>(rand() % MAX_CODE) / MAX_CODE);
  }
};

#endif // IPS_TEST_ADC_HPP
