#ifndef IPS_DAC_MODEL_HPP
#define IPS_DAC_MODEL_HPP

#include <systemc-ams.h>
#include "vunit.hpp"


template <unsigned int BITS = 8, int VMIN = 0, int VMAX = 5, VUnit VU = VUnit::v>
SCA_TDF_MODULE(dac)
{
protected:
  const double V_MAX = static_cast<double>(VMAX) / static_cast<double>(VU);
  const double V_MIN = static_cast<double>(VMIN) / static_cast<double>(VU);
  const double MAX_DIG = static_cast<double>((1 << BITS) - 1);
public:
  sca_tdf::sca_in<sc_dt::sc_uint<BITS> > in;
  sca_tdf::sca_out<double> out;

  SCA_CTOR(dac) : in("in"), out("out") {
    set_timestep(sca_core::sca_time(0.1, sc_core::SC_US));
  }

  void processing()
  {
    double dig_in = static_cast<double>(this->in.read().to_uint());
    double ana_out = V_MIN + (dig_in / MAX_DIG) * (V_MAX - V_MIN);

    this->out.write(ana_out);
  }
};
#endif // IPS_DAC_MODEL_HPP
