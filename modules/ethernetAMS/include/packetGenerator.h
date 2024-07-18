#include <systemc.h>
#include <systemc-ams.h>

// Clock generator module
SC_MODULE(clock_generator)
{
  sc_out<bool> clk_out;
  sc_time period;
  
  void generate_clock() {
    while (true) {
      clk_out.write(true);
      wait(period / 2);
      clk_out.write(false);
      wait(period / 2);
    }
  }

  SC_CTOR(clock_generator) {
    SC_THREAD(generate_clock);
    period = sc_time(10, SC_NS); // Set clock period to 10 nanoseconds
  }
};

SC_MODULE(packetGenerator)
{
  SC_CTOR(packetGenerator) : bitCount(0)
  {
    SC_METHOD(generate_packet);
    sensitive << cclk.pos();
  }
  
  #define N 16
  
  sc_in<bool> cclk;
  sc_out<bool> data_out_valid;
  sc_out<sc_dt::sc_bv<4>> data_out;
  
  sc_signal<bool> next_data_out_valid;
  sc_signal<sc_dt::sc_bv<4>> next_data_out;
  
  sc_signal<sc_dt::sc_bv<N * 4>> data_in;
  sc_signal<sc_dt::sc_bv<N>> data_in_valid;
  sc_signal<sc_dt::sc_bv<N>> next_data_in_valid;
  sc_signal<sc_dt::sc_int<4>> sigBitCount;
  
  int bitCount;
  
  void generate_packet();
  
  void fill_data(unsigned char* data, int nibbles_length);
};
