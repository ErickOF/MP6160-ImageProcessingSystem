#ifndef IPS_SEQ_ITEM_ADC_HPP
#define IPS_SEQ_ITEM_ADC_HPP

#include <cstdlib>


/**
 * @brief This class is used to generate the analog signal for the test
 * 
 * @tparam N - the number of output bits of the digital code
 * @tparam VMIN - lowest voltage value
 * @tparam VMAX - highest voltage value
 * @tparam VU - voltage unit based on VUnit
 */
template <unsigned int N = 8, int VMIN = 0, int VMAX = 5, VUnit VU = VUnit::v>
SCA_TDF_MODULE(seq_item_adc)
{
protected:
  // Min voltage value based on the voltage units
  const double V_MIN = static_cast<double>(VMIN) / static_cast<double>(VU);
  // Max voltage value based on the voltage units
  const double V_MAX = static_cast<double>(VMAX) / static_cast<double>(VU);
  // Max digital output code
  const int MAX_CODE = (1 << N);
public:
  sca_tdf::sca_out<double> o_ana;

  SCA_CTOR(seq_item_adc)
  {
    set_timestep(sca_core::sca_time(13, sc_core::SC_NS));
  }

  void processing()
  {
    const double NORM_ANA = static_cast<double>(rand() % MAX_CODE) / MAX_CODE;
    this->o_ana.write((V_MAX + V_MIN) * NORM_ANA + V_MIN);
  }
};

#endif // IPS_SEQ_ITEM_ADC_HPP
