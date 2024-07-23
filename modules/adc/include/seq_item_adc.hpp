#ifndef IPS_SEQ_ITEM_ADC_HPP
#define IPS_SEQ_ITEM_ADC_HPP

#include <cstdlib>


/**
 * @brief This class is used to generate the analog signal for the test
 * 
 * @tparam N 
 */
template <unsigned int N>
SCA_TDF_MODULE(seq_item_adc)
{
public:
  sca_tdf::sca_out<double> o_ana;
  const int MAX_CODE = (1 << N);

  SCA_CTOR(seq_item_adc)
  {
    set_timestep(sca_core::sca_time(0.1, sc_core::SC_US));
  }

  void processing()
  {
    this->o_ana.write(static_cast<double>(rand() % MAX_CODE) / MAX_CODE);
  }
};

#endif // IPS_SEQ_ITEM_ADC_HPP
