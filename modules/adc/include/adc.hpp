#ifndef IPS_ADC_MODEL_HPP
#define IPS_ADC_MODEL_HPP

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
  sca_tdf::sca_in<double> in;
  sca_tdf::sca_out<sc_dt::sc_uint<BITS> > out;

  SCA_CTOR(dac) : in("in"), out("out") {
    set_timestep(sca_core::sca_time(0.1, sc_core::SC_US));
  }

  void processing()
  {
    double normalized_ana_in = (in.read() - V_MIN) / (V_MAX - V_MIN);
    unsigned int dig_code = static_cast<unsigned int>(normalized_ana_in * MAX_DIG);

    this->out.write(static_cast<sc_dt::sc_uint<BITS> >(dig_code));
  }
};

#endif // IPS_ADC_MODEL_HPP
