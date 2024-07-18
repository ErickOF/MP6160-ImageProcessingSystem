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

    preamble_in_process = false;
    data_length_in_process = false;
  }

  #define N 16

  // Main results from block
  sca_tdf::sca_out<bool> data_out_valid;
  sca_tdf::sca_out<sc_dt::sc_bv<4>> data_out;

  // Signals for tracing the values
  sca_tdf::sca_out<bool> tmp_data_out_valid_;

  sca_tdf::sca_out<bool> n1_data_out_valid_;
  sca_tdf::sca_out<sc_dt::sc_bv<4>> n1_data_out_;
  sca_tdf::sca_out<sc_dt::sc_bv<N>> n1_data_valid_;

  sca_tdf::sca_out<bool> n2_data_out_valid_;
  sca_tdf::sca_out<sc_dt::sc_bv<4>> n2_data_out_;
  sca_tdf::sca_out<sc_dt::sc_bv<N>> n2_data_valid_;

  sca_tdf::sca_out<sc_dt::sc_bv<N * 4>> data_in_;
  sca_tdf::sca_out<sc_dt::sc_bv<N>> data_in_valid_;

  sca_tdf::sca_out<sc_dt::sc_bv<N * 4>> data_to_send_;
  sca_tdf::sca_out<sc_dt::sc_bv<N>> data_valid_to_send_;

  sca_tdf::sca_out<sc_dt::sc_int<4>> n1_sigBitCount_;
  sca_tdf::sca_out<sc_dt::sc_int<4>> n2_sigBitCount_;
  sca_tdf::sca_out<sc_dt::sc_int<4>> sigBitCount;

  // Internal values to do the logic
  bool tmp_data_out_valid;

  bool n1_data_out_valid;
  sc_dt::sc_bv<4> n1_data_out;
  sc_dt::sc_bv<N> n1_data_valid;
  sc_dt::sc_int<4> n1_sigBitCount;

  bool n2_data_out_valid;
  sc_dt::sc_bv<4> n2_data_out;
  sc_dt::sc_bv<N> n2_data_valid;
  sc_dt::sc_int<4> n2_sigBitCount;

  bool preamble_in_process;
  bool data_length_in_process;

  // Data to be sent, set in fill_data()
  sc_dt::sc_bv<N * 4> data_in;
  sc_dt::sc_bv<N> data_in_valid;

  sc_dt::sc_bv<N * 4> data_to_send;
  sc_dt::sc_bv<N> data_valid_to_send;

  int bitCount;

  void fill_data(unsigned char* data, int data_length);

  void processing();

};
