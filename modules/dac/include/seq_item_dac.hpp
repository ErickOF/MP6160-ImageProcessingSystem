#ifndef IPS_SEQ_ITEM_DAC_HPP
#define IPS_SEQ_ITEM_DAC_HPP

#include <cstdlib>


/**
 * @brief This class is used to generate the digital code for the test
 * 
 * @tparam N 
 */
template <unsigned int N>
SCA_TDF_MODULE(seq_item_dac)
{
public:
  sca_tdf::sca_out<sc_dt::sc_uint<N> > o_dig;

  SCA_CTOR(seq_item_dac)
  {
    set_timestep(sca_core::sca_time(17, sc_core::SC_NS));
  }

  void processing()
  {
    this->o_dig.write(static_cast<sc_dt::sc_uint<N> >(rand() % (1 << N)));
  }
};

#endif // IPS_SEQ_ITEM_DAC_HPP
