#ifndef IPS_ADC_MODEL_HPP
#define IPS_ADC_MODEL_HPP

#include <systemc-ams.h>
#include "vunit.hpp"


/**
 * @brief Analog to Digital Converter module representation
 * This module generates a N-bit digital signal based on the [Vmin, Vmax]
 * voltage range
 * 
 * @tparam BITS - the number of output bits of the digital code
 * @tparam VMIN - lowest voltage value
 * @tparam VMAX - highest voltage value
 * @tparam VU - voltage unit based on VUnit
 */
template <unsigned int BITS = 8, int VMIN = 0, int VMAX = 5, VUnit VU = VUnit::v>
SCA_TDF_MODULE(adc)
{
protected:
  // Min voltage value based on the voltage units
  const double V_MIN = static_cast<double>(VMIN) / static_cast<double>(VU);
  // Max voltage value based on the voltage units
  const double V_MAX = static_cast<double>(VMAX) / static_cast<double>(VU);
  // Max digital output code
  const double MAX_DIG = static_cast<double>((1 << BITS) - 1);
public:
  // Input analog voltage
  sca_tdf::sca_in<double> in;
  // Output digital code
  sca_tdf::sca_out<sc_dt::sc_uint<BITS> > out;

  /**
   * @brief Construct a new adc object
   * 
   */
  SCA_CTOR(adc) : in("in"), out("out") {
    // Propagation time from input to output
    set_timestep(sca_core::sca_time(0.1, sc_core::SC_US));
  }

  /**
   * @brief Convert the analog signal into digital signal
   * The analog signal in a range from Vmin to Vmax is converted into a N-bit
   * digital signal
   * 
   */
  void processing()
  {

    double normalized_ana_in = (in.read() - V_MIN) / (V_MAX - V_MIN);
    unsigned int dig_code = static_cast<unsigned int>(normalized_ana_in * MAX_DIG);

    this->out.write(static_cast<sc_dt::sc_uint<BITS> >(dig_code));
  }
};

#endif // IPS_ADC_MODEL_HPP
