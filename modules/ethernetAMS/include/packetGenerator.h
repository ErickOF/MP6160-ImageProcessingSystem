#include <systemc-ams.h>

SCA_TDF_MODULE(packetGenerator)
{
  SCA_CTOR(packetGenerator) : bitCount(0)
  {
    // Set the time-step
    set_timestep(sc_core::sc_time(10, sc_core::SC_NS));
    tmp_data_out_valid = false;
    n1_data_out_valid = false;
    n2_data_out_valid = false;
    n2_sigBitCount = 0;
  }

  #define N 16

  sca_tdf::sca_out<bool> data_out_valid;
  sca_tdf::sca_out<sc_dt::sc_bv<4>> data_out;

  sca_tdf::sca_out<bool> tmp_data_out_valid_;

  sca_tdf::sca_out<bool> n1_data_out_valid_;
  sca_tdf::sca_out<sc_dt::sc_bv<4>> n1_data_out_;
  sca_tdf::sca_out<sc_dt::sc_bv<N>> n1_data_in_valid_;

  sca_tdf::sca_out<bool> n2_data_out_valid_;
  sca_tdf::sca_out<sc_dt::sc_bv<4>> n2_data_out_;
  sca_tdf::sca_out<sc_dt::sc_bv<N>> n2_data_in_valid_;

  sca_tdf::sca_out<sc_dt::sc_bv<N * 4>> data_in_;
  sca_tdf::sca_out<sc_dt::sc_bv<N>> data_in_valid_;
  sca_tdf::sca_out<sc_dt::sc_int<4>> n1_sigBitCount_;
  sca_tdf::sca_out<sc_dt::sc_int<4>> n2_sigBitCount_;
  sca_tdf::sca_out<sc_dt::sc_int<4>> sigBitCount;

  bool tmp_data_out_valid;

  bool n1_data_out_valid;
  sc_dt::sc_bv<4> n1_data_out;
  sc_dt::sc_bv<N> n1_data_in_valid;
  sc_dt::sc_int<4> n1_sigBitCount;

  bool n2_data_out_valid;
  sc_dt::sc_bv<4> n2_data_out;
  sc_dt::sc_bv<N> n2_data_in_valid;
  sc_dt::sc_int<4> n2_sigBitCount;

  sc_dt::sc_bv<N * 4> data_in;
  sc_dt::sc_bv<N> data_in_valid;

  int bitCount;

  void fill_data(unsigned char* data, int data_length);
  void generate_packet();
  void processing();

  // Removed sc_in<bool> cclk;
};
