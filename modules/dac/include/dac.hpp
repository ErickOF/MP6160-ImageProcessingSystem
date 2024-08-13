#ifndef IPS_DAC_MODEL_HPP
#define IPS_DAC_MODEL_HPP

#include <systemc-ams.h>
#include "vunit.hpp"


/**
 * @brief Digital to Analog Converter module representation
 * This module generates an analog signal based on the [Vmin, Vmax] voltage
 * range
 * 
 * @tparam BITS - the number of output bits of the digital code
 * @tparam VMIN - lowest voltage value
 * @tparam VMAX - highest voltage value
 * @tparam VU - voltage unit based on VUnit
 */
template <unsigned int BITS = 8, int VMIN = 0, int VMAX = 5, VUnit VU = VUnit::v>
SCA_TDF_MODULE(dac)
{
protected:
  // Min voltage value based on the voltage units
  const double V_MIN = static_cast<double>(VMIN) / static_cast<double>(VU);
  // Max voltage value based on the voltage units
  const double V_MAX = static_cast<double>(VMAX) / static_cast<double>(VU);
  // Max digital output code
  const double MAX_DIG = static_cast<double>((1 << BITS) - 1);
public:
  // Input digital code
  sca_tdf::sca_de::sca_in<sc_dt::sc_uint<BITS> > in;
  // Output analog voltage
  sca_tdf::sca_out<double> out;

  /**
   * @brief Construct a new dac object
   * 
   */
  SCA_CTOR(dac) : in("in"), out("out")
  {
  }

  void set_attributes()
  {
    // Propagation time from input to output
    set_timestep(sca_core::sca_time(1, sc_core::SC_NS));
    this->out.set_delay(17);
  }

  /**
   * @brief Convert the digital signal into analog signal
   * The N-bit digital code is converted into an analog signal in a voltage
   * range from Vmin to Vmax
   * 
   */
  void processing()
  {
    double dig_in = static_cast<double>(this->in.read().to_uint());
    double ana_out = V_MIN + (dig_in / MAX_DIG) * (V_MAX - V_MIN);

    this->out.write(ana_out);
  }
};

#endif // IPS_DAC_MODEL_HPP
