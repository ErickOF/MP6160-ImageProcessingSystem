#ifndef IPS_FILTER_HPP
#define IPS_FILTER_HPP

#include <systemc.h>

#define N 3 //Filter Kernel

SC_MODULE (Filter) {
  //-----------------------------Local Variables-----------------------------
    // float kernel[N*N];
    sc_in<sc_uint<8> > input_window_0;
    sc_in<sc_uint<8> > input_window_1;
    sc_in<sc_uint<8> > input_window_2;
    sc_in<sc_uint<8> > input_window_3;
    sc_in<sc_uint<8> > input_window_4;
    sc_in<sc_uint<8> > input_window_5;
    sc_in<sc_uint<8> > input_window_6;
    sc_in<sc_uint<8> > input_window_7;
    sc_in<sc_uint<8> > input_window_8;
    sc_out<sc_uint<8> > output;

    sc_in<bool> clk;
    
    SC_HAS_PROCESS(Filter);
    Filter(sc_module_name name)
    {
      SC_CTHREAD(filter, clk.pos());
      // SC_METHOD(filter);
      // sensitive << input_window_0 << input_window_1 << input_window_2 << input_window_3 << input_window_4 << input_window_5 << input_window_6 << input_window_7 << input_window_8;
      // for (int i = 0; i < N*N; is++){
      //   kernel[i] = 1/N;
      // }
    }


    void filter();
    // void init_kernel();

};


#endif // IPS_FILTER_HPP